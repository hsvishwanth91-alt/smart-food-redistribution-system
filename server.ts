import express from 'express';
import dotenv from 'dotenv';
import path from 'path';
import { fileURLToPath } from 'url';
import { GoogleGenAI } from '@google/genai';

dotenv.config();

const __filename = fileURLToPath(import.meta.url);
const __dirname = path.dirname(__filename);

const app = express();
const port = parseInt(process.env.PORT || '3000', 10);

app.use(express.json({ limit: '50mb' }));

// Server-side Gemini client initialization
const ai = new GoogleGenAI({
  apiKey: process.env.GEMINI_API_KEY,
  httpOptions: {
    headers: {
      'User-Agent': 'aistudio-build',
    },
  },
});

// Helper: Clean SVG code from model response
function extractCleanSvg(text: string): string {
  let cleaned = text.trim();
  // Remove markdown code fence if present
  if (cleaned.startsWith('```')) {
    cleaned = cleaned.replace(/^```(?:xml|svg)?\s*/i, '').replace(/\s*```$/, '');
  }
  const svgStart = cleaned.indexOf('<svg');
  const svgEnd = cleaned.lastIndexOf('</svg>');
  if (svgStart !== -1 && svgEnd !== -1) {
    return cleaned.substring(svgStart, svgEnd + 6);
  }
  return cleaned;
}

// Health check endpoint
app.get('/api/health', (_req, res) => {
  res.json({ status: 'ok', models: ['gemini-3.8-flash', 'gemini-nano-banana-2.1'] });
});

// Generate Image Endpoint
// Supports both gemini-3.8-flash (Vector/SVG High Detail, Free Tier)
// and gemini-nano-banana-2.1 (Raster/Paid Tier with graceful fallback)
app.post('/api/images/generate', async (req, res) => {
  try {
    const { prompt, aspectRatio = '1:1', imageSize = '1K', preferredEngine = 'auto' } = req.body;
    if (!prompt || typeof prompt !== 'string') {
      return res.status(400).json({ error: 'Text prompt is required' });
    }

    // If user explicitly asks for nano-banana, try it first
    if (preferredEngine === 'nano-banana') {
      try {
        const response = await ai.models.generateContent({
          model: 'gemini-nano-banana-2.1',
          contents: {
            parts: [{ text: prompt }],
          },
          config: {
            imageConfig: {
              aspectRatio: aspectRatio,
              imageSize: imageSize,
            },
          },
        });

        for (const part of response.candidates?.[0]?.content?.parts ?? []) {
          if (part.inlineData?.data) {
            const base64 = part.inlineData.data;
            const mime = part.inlineData.mimeType || 'image/png';
            return res.json({
              imageUrl: `data:${mime};base64,${base64}`,
              format: 'raster',
              engine: 'gemini-nano-banana-2.1',
              prompt,
            });
          }
        }
      } catch (nanoErr: any) {
        console.warn('nano-banana failed or requires billing, falling back to gemini-3.8-flash vector engine:', nanoErr.message);
      }
    }

    // Default & Free Tier Engine: gemini-3.8-flash Vector/SVG Digital Art & Graphic Generator
    const [width, height] = aspectRatio === '16:9' ? [960, 540]
      : aspectRatio === '4:3' ? [800, 600]
      : aspectRatio === '9:16' ? [540, 960]
      : aspectRatio === '3:4' ? [600, 800]
      : aspectRatio === '4:1' ? [1200, 300]
      : [800, 800];

    const systemPrompt = `You are a world-class digital graphic artist, UI/UX illustrator, and vector designer.
Your task is to generate a visually stunning, high-detail, modern SVG image based on the user's prompt.
Strict Technical Requirements:
1. Output ONLY valid, standalone SVG code starting with <svg> and ending with </svg>.
2. Do NOT enclose in markdown code blocks (\`\`\`). Do NOT include any explanations or commentary.
3. Include standard attributes: viewBox="0 0 ${width} ${height}" width="100%" height="100%" xmlns="http://www.w3.org/2000/svg".
4. Use rich graphic elements: <defs> with linear and radial gradients, subtle drop shadows (<filter>), modern rounded shapes, multi-layered depth, crisp typography where appropriate, and a cohesive, professional color palette (emerald, teal, amber, slate, coral, gold).
5. Ensure the graphic directly and artistically depicts the requested theme in full fidelity.`;

    const flashResponse = await ai.models.generateContent({
      model: 'gemini-3.8-flash',
      contents: prompt,
      config: {
        systemInstruction: systemPrompt,
      },
    });

    const rawSvg = flashResponse.text || '';
    const cleanSvg = extractCleanSvg(rawSvg);

    if (!cleanSvg.startsWith('<svg')) {
      return res.status(500).json({
        error: 'Failed to synthesize vector image',
        details: rawSvg,
      });
    }

    const base64Svg = Buffer.from(cleanSvg, 'utf-8').toString('base64');
    const imageUrl = `data:image/svg+xml;base64,${base64Svg}`;

    return res.json({
      imageUrl,
      svgCode: cleanSvg,
      format: 'svg',
      engine: 'gemini-3.8-flash',
      prompt,
    });
  } catch (err: any) {
    console.error('Server error generating image:', err);
    return res.status(err.status || 500).json({
      error: err.message || 'Failed to generate image',
      details: err.toString(),
    });
  }
});

// Edit Image Endpoint
// Allows modifying an existing SVG image or synthesizing an inpainting transformation
app.post('/api/images/edit', async (req, res) => {
  try {
    const { prompt, imageBase64, svgCode, preferredEngine = 'auto' } = req.body;
    if (!prompt) {
      return res.status(400).json({ error: 'Modification instruction prompt is required' });
    }

    // If source is SVG (from our generator or pasted SVG code)
    if (svgCode || (imageBase64 && imageBase64.includes('image/svg+xml'))) {
      let currentSvg = svgCode;
      if (!currentSvg && imageBase64) {
        const base64Part = imageBase64.split(',')[1] || imageBase64;
        currentSvg = Buffer.from(base64Part, 'base64').toString('utf-8');
      }

      const editSystemPrompt = `You are an expert digital illustrator and SVG designer.
You are given an existing SVG graphic and user instructions to edit/modify it.
Carefully apply the requested changes (such as adding badges, changing color schemes, updating text, modifying shapes, or adding decorative elements) while preserving the overarching layout and structure.
Output ONLY the modified, valid, standalone SVG code starting with <svg> and ending with </svg>. No markdown formatting or commentary.`;

      const response = await ai.models.generateContent({
        model: 'gemini-3.8-flash',
        contents: `Original SVG:\n${currentSvg}\n\nUser Modification Request: "${prompt}"\n\nPlease output the modified SVG:`,
        config: {
          systemInstruction: editSystemPrompt,
        },
      });

      const updatedRawSvg = response.text || '';
      const updatedCleanSvg = extractCleanSvg(updatedRawSvg);

      if (!updatedCleanSvg.startsWith('<svg')) {
        return res.status(500).json({
          error: 'Failed to apply edits to vector image',
          details: updatedRawSvg,
        });
      }

      const base64Svg = Buffer.from(updatedCleanSvg, 'utf-8').toString('base64');
      const imageUrl = `data:image/svg+xml;base64,${base64Svg}`;

      return res.json({
        imageUrl,
        svgCode: updatedCleanSvg,
        format: 'svg',
        engine: 'gemini-3.8-flash',
        prompt,
      });
    }

    // For raster images, if nano-banana is requested
    if (preferredEngine === 'nano-banana' && imageBase64) {
      try {
        const cleanBase64 = imageBase64.includes(',') ? imageBase64.split(',')[1] : imageBase64;
        const response = await ai.models.generateContent({
          model: 'gemini-nano-banana-2.1',
          contents: {
            parts: [
              {
                inlineData: {
                  data: cleanBase64,
                  mimeType: 'image/png',
                },
              },
              { text: prompt },
            ],
          },
        });

        for (const part of response.candidates?.[0]?.content?.parts ?? []) {
          if (part.inlineData?.data) {
            const base64 = part.inlineData.data;
            const mime = part.inlineData.mimeType || 'image/png';
            return res.json({
              imageUrl: `data:${mime};base64,${base64}`,
              format: 'raster',
              engine: 'gemini-nano-banana-2.1',
              prompt,
            });
          }
        }
      } catch (nanoErr: any) {
        console.warn('nano-banana edit fallback to flash:', nanoErr.message);
      }
    }

    // Fallback: Use gemini-3.8-flash to interpret the prompt and create an enhanced version
    const fallbackResponse = await ai.models.generateContent({
      model: 'gemini-3.8-flash',
      contents: `Create an updated graphic incorporating: "${prompt}" based on the existing image context.`,
      config: {
        systemInstruction: `You are an expert digital graphic designer. Output ONLY valid, standalone SVG code starting with <svg> and ending with </svg>. No markdown wrappers.`,
      },
    });

    const fallbackCleanSvg = extractCleanSvg(fallbackResponse.text || '');
    const base64Svg = Buffer.from(fallbackCleanSvg, 'utf-8').toString('base64');
    return res.json({
      imageUrl: `data:image/svg+xml;base64,${base64Svg}`,
      svgCode: fallbackCleanSvg,
      format: 'svg',
      engine: 'gemini-3.8-flash',
      prompt,
    });
  } catch (err: any) {
    console.error('Server error editing image:', err);
    return res.status(err.status || 500).json({
      error: err.message || 'Failed to edit image',
      details: err.toString(),
    });
  }
});

// Vite Middleware for SPA development and production static serving
async function startServer() {
  if (process.env.NODE_ENV === 'production') {
    app.use(express.static(path.resolve(__dirname, 'dist')));
    app.get('*', (_req, res) => {
      res.sendFile(path.resolve(__dirname, 'dist', 'index.html'));
    });
  } else {
    const { createServer: createViteServer } = await import('vite');
    const vite = await createViteServer({
      server: { middlewareMode: true, hmr: process.env.DISABLE_HMR !== 'true' },
      appType: 'spa',
    });
    app.use(vite.middlewares);
  }

  app.listen(port, '0.0.0.0', () => {
    console.log(`[Express + Vite] Server running on http://0.0.0.0:${port}`);
  });
}

startServer();
