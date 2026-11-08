import React, { useState, useRef } from 'react';
import {
  Sparkles,
  Image as ImageIcon,
  Wand2,
  Download,
  Upload,
  RefreshCw,
  Copy,
  Check,
  Sliders,
  ArrowRight,
  Info,
  AlertCircle,
  Trash2,
  FileCode,
} from 'lucide-react';

interface GeneratedItem {
  id: string;
  type: 'generate' | 'edit';
  prompt: string;
  imageUrl: string;
  svgCode?: string;
  format?: 'svg' | 'raster';
  originalImageUrl?: string;
  timestamp: string;
  aspectRatio: string;
  imageSize: string;
  engine?: string;
}

export const ImageStudio: React.FC = () => {
  const [activeSubTab, setActiveSubTab] = useState<'generate' | 'edit'>('generate');
  
  // Generation State
  const [genPrompt, setGenPrompt] = useState<string>(
    'Modern community food redistribution center with volunteers packing fresh produce boxes, emerald green and warm gold aesthetic, clean vector illustration'
  );
  const [aspectRatio, setAspectRatio] = useState<string>('1:1');
  const [imageSize, setImageSize] = useState<string>('1K');
  const [isGenerating, setIsGenerating] = useState<boolean>(false);
  const [genError, setGenError] = useState<string | null>(null);

  // Editing State
  const [editPrompt, setEditPrompt] = useState<string>(
    'Add an eco-friendly green "CERTIFIED ZERO-WASTE" badge with a leaf icon in the top right corner'
  );
  const [sourceImage, setSourceImage] = useState<string | null>(null);
  const [sourceSvgCode, setSourceSvgCode] = useState<string | null>(null);
  const [isEditing, setIsEditing] = useState<boolean>(false);
  const [editError, setEditError] = useState<string | null>(null);

  // Gallery History
  const [history, setHistory] = useState<GeneratedItem[]>([]);
  const [activeItem, setActiveItem] = useState<GeneratedItem | null>(null);
  const [copied, setCopied] = useState<boolean>(false);

  const fileInputRef = useRef<HTMLInputElement>(null);

  const promptPresets = [
    {
      label: 'Fresh Produce Donation',
      prompt: 'Fresh organic apples, broccoli, and carrots packed in sustainable wooden crates for community food redistribution, crisp sunlight, clean vector graphics',
    },
    {
      label: 'Emergency Meal Packs',
      prompt: 'Clean nutritious cooked meal parcels with expiry tracking labels in a hygienic staging kitchen, high-detail illustration',
    },
    {
      label: 'Zero-Waste Logo Badge',
      prompt: 'Minimalist modern vector emblem for a Smart Food Redistribution initiative, emerald green and gold, clean lines',
    },
    {
      label: 'Refrigerated Delivery Van',
      prompt: 'Eco-friendly electric refrigerated food delivery van arriving at a vibrant community center on a sunny morning',
    },
  ];

  const editPresets = [
    'Add an urgent red "PRIORITY DISPATCH" stamp on the box',
    'Add an eco-friendly green "ZERO-WASTE VERIFIED" badge with a leaf emblem',
    'Adjust lighting to warm golden hour sunshine with soft shadows',
    'Add a digital holographic barcode and expiry tracking label',
  ];

  const handleGenerate = async () => {
    if (!genPrompt.trim()) return;
    setIsGenerating(true);
    setGenError(null);

    try {
      const res = await fetch('/api/images/generate', {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify({
          prompt: genPrompt,
          aspectRatio,
          imageSize,
        }),
      });

      const data = await res.json();
      if (!res.ok) {
        throw new Error(data.error || data.details || 'Failed to generate image');
      }

      const newItem: GeneratedItem = {
        id: 'img-' + Date.now(),
        type: 'generate',
        prompt: genPrompt,
        imageUrl: data.imageUrl,
        svgCode: data.svgCode,
        format: data.format || 'svg',
        engine: data.engine || 'gemini-3.8-flash',
        timestamp: new Date().toLocaleTimeString(),
        aspectRatio,
        imageSize,
      };

      setHistory((prev) => [newItem, ...prev]);
      setActiveItem(newItem);
    } catch (err: any) {
      console.error(err);
      setGenError(err.message || 'An error occurred while generating image');
    } finally {
      setIsGenerating(false);
    }
  };

  const handleEdit = async () => {
    if (!sourceImage && !sourceSvgCode) {
      setEditError('Please upload or select a source image to edit.');
      return;
    }
    if (!editPrompt.trim()) {
      setEditError('Please provide an edit prompt describing what changes to make.');
      return;
    }

    setIsEditing(true);
    setEditError(null);

    try {
      const res = await fetch('/api/images/edit', {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify({
          prompt: editPrompt,
          imageBase64: sourceImage,
          svgCode: sourceSvgCode,
        }),
      });

      const data = await res.json();
      if (!res.ok) {
        throw new Error(data.error || data.details || 'Failed to edit image');
      }

      const newItem: GeneratedItem = {
        id: 'edit-' + Date.now(),
        type: 'edit',
        prompt: editPrompt,
        imageUrl: data.imageUrl,
        svgCode: data.svgCode,
        format: data.format || 'svg',
        engine: data.engine || 'gemini-3.8-flash',
        originalImageUrl: sourceImage || (sourceSvgCode ? `data:image/svg+xml;base64,${btoa(sourceSvgCode)}` : undefined),
        timestamp: new Date().toLocaleTimeString(),
        aspectRatio,
        imageSize,
      };

      setHistory((prev) => [newItem, ...prev]);
      setActiveItem(newItem);
    } catch (err: any) {
      console.error(err);
      setEditError(err.message || 'An error occurred while editing image');
    } finally {
      setIsEditing(false);
    }
  };

  const handleFileUpload = (e: React.ChangeEvent<HTMLInputElement>) => {
    const file = e.target.files?.[0];
    if (!file) return;

    if (!file.type.startsWith('image/')) {
      setEditError('Please upload a valid image file (SVG, PNG, JPEG, WebP).');
      return;
    }

    if (file.type.includes('svg')) {
      const textReader = new FileReader();
      textReader.onload = (event) => {
        if (typeof event.target?.result === 'string') {
          setSourceSvgCode(event.target.result);
          setSourceImage(URL.createObjectURL(file));
          setEditError(null);
        }
      };
      textReader.readAsText(file);
    } else {
      const reader = new FileReader();
      reader.onload = (event) => {
        if (typeof event.target?.result === 'string') {
          setSourceImage(event.target.result);
          setSourceSvgCode(null);
          setEditError(null);
        }
      };
      reader.readAsDataURL(file);
    }
  };

  const handleDownload = (imageUrl: string, filename: string) => {
    const a = document.createElement('a');
    a.href = imageUrl;
    a.download = filename;
    document.body.appendChild(a);
    a.click();
    document.body.removeChild(a);
  };

  const handleSendToEditor = (item: GeneratedItem) => {
    setSourceImage(item.imageUrl);
    setSourceSvgCode(item.svgCode || null);
    setActiveSubTab('edit');
    window.scrollTo({ top: 0, behavior: 'smooth' });
  };

  const handleCopySvg = () => {
    if (activeItem?.svgCode) {
      navigator.clipboard.writeText(activeItem.svgCode);
      setCopied(true);
      setTimeout(() => setCopied(false), 2000);
    }
  };

  return (
    <div className="space-y-6">
      {/* Studio Header Card */}
      <div className="bg-white rounded-xl border border-slate-200 p-5 shadow-xs">
        <div className="flex flex-col sm:flex-row items-start sm:items-center justify-between gap-4">
          <div className="flex items-center gap-3">
            <div className="w-10 h-10 rounded-xl bg-gradient-to-br from-indigo-500 to-cyan-600 flex items-center justify-center text-white shadow-md shadow-indigo-500/20">
              <Sparkles className="w-5 h-5" />
            </div>
            <div>
              <div className="flex items-center gap-2">
                <h2 className="text-base font-bold text-slate-900 tracking-tight">
                  AI Image Studio
                </h2>
                <span className="px-2 py-0.5 rounded-full text-[10px] font-mono font-bold bg-indigo-50 text-indigo-700 border border-indigo-200">
                  gemini-3.8-flash Vector Studio
                </span>
                <span className="px-2 py-0.5 rounded-full text-[10px] font-mono font-bold bg-emerald-50 text-emerald-700 border border-emerald-200">
                  Ready &amp; Active
                </span>
              </div>
              <p className="text-xs text-slate-500 mt-0.5">
                Generate high-resolution food donation visuals, badges, and delivery logistics art, or edit existing graphics with text prompt instructions.
              </p>
            </div>
          </div>

          {/* Sub-tab Switcher */}
          <div className="flex items-center bg-slate-100 p-1 rounded-lg border border-slate-200 text-xs font-semibold">
            <button
              onClick={() => setActiveSubTab('generate')}
              className={`px-3 py-1.5 rounded-md flex items-center gap-1.5 transition-all cursor-pointer ${
                activeSubTab === 'generate'
                  ? 'bg-white text-indigo-700 shadow-xs'
                  : 'text-slate-600 hover:text-slate-900'
              }`}
            >
              <Wand2 className="w-3.5 h-3.5" />
              <span>Create Image</span>
            </button>
            <button
              onClick={() => setActiveSubTab('edit')}
              className={`px-3 py-1.5 rounded-md flex items-center gap-1.5 transition-all cursor-pointer ${
                activeSubTab === 'edit'
                  ? 'bg-white text-indigo-700 shadow-xs'
                  : 'text-slate-600 hover:text-slate-900'
              }`}
            >
              <Sliders className="w-3.5 h-3.5" />
              <span>Edit Existing Image</span>
            </button>
          </div>
        </div>
      </div>

      {/* Main Studio Workspace */}
      <div className="grid grid-cols-1 lg:grid-cols-12 gap-6">
        {/* Controls Column (5 cols) */}
        <div className="lg:col-span-5 space-y-4">
          {activeSubTab === 'generate' ? (
            /* GENERATION PANEL */
            <div className="bg-white rounded-xl border border-slate-200 p-5 shadow-xs space-y-4">
              <div className="flex items-center justify-between">
                <span className="text-xs font-bold text-slate-800 uppercase tracking-wider flex items-center gap-1.5">
                  <Wand2 className="w-3.5 h-3.5 text-indigo-600" />
                  Text Prompt
                </span>
                <span className="text-[11px] text-slate-400 font-mono">gemini-3.8-flash</span>
              </div>

              {/* Prompt Input Area */}
              <div>
                <textarea
                  value={genPrompt}
                  onChange={(e) => setGenPrompt(e.target.value)}
                  rows={4}
                  placeholder="Describe the image you want to create..."
                  className="w-full bg-slate-50 border border-slate-300 rounded-lg p-3 text-xs text-slate-900 focus:outline-none focus:border-indigo-600 focus:bg-white transition-all shadow-inner"
                />
              </div>

              {/* Prompt Presets */}
              <div className="space-y-1.5">
                <span className="text-[11px] font-semibold text-slate-600">Sample Presets:</span>
                <div className="flex flex-wrap gap-1.5">
                  {promptPresets.map((preset, idx) => (
                    <button
                      key={idx}
                      onClick={() => setGenPrompt(preset.prompt)}
                      className="px-2 py-1 rounded-md bg-slate-100 hover:bg-indigo-50 hover:text-indigo-700 border border-slate-200 text-[11px] text-slate-700 transition-all cursor-pointer"
                    >
                      {preset.label}
                    </button>
                  ))}
                </div>
              </div>

              {/* Aspect Ratio & Format */}
              <div className="grid grid-cols-2 gap-3 pt-2 border-t border-slate-100">
                <div>
                  <label className="block text-[11px] font-semibold text-slate-700 mb-1">
                    Aspect Ratio
                  </label>
                  <select
                    value={aspectRatio}
                    onChange={(e) => setAspectRatio(e.target.value)}
                    className="w-full bg-slate-50 border border-slate-300 rounded-lg px-2.5 py-1.5 text-xs text-slate-800 focus:outline-none focus:border-indigo-600"
                  >
                    <option value="1:1">1:1 Square</option>
                    <option value="16:9">16:9 Landscape</option>
                    <option value="4:3">4:3 Standard</option>
                    <option value="9:16">9:16 Portrait</option>
                    <option value="3:4">3:4 Vertical</option>
                    <option value="4:1">4:1 Panoramic Banner</option>
                  </select>
                </div>

                <div>
                  <label className="block text-[11px] font-semibold text-slate-700 mb-1">
                    Rendering Format
                  </label>
                  <select
                    value={imageSize}
                    onChange={(e) => setImageSize(e.target.value)}
                    className="w-full bg-slate-50 border border-slate-300 rounded-lg px-2.5 py-1.5 text-xs text-slate-800 focus:outline-none focus:border-indigo-600"
                  >
                    <option value="1K">Scalable Vector / High Detail</option>
                    <option value="2K">2K Quad Detail</option>
                  </select>
                </div>
              </div>

              {/* Error Banner */}
              {genError && (
                <div className="p-3 rounded-lg bg-rose-50 border border-rose-200 text-rose-700 text-xs flex items-start gap-2">
                  <AlertCircle className="w-4 h-4 shrink-0 mt-0.5" />
                  <div>
                    <div className="font-bold">Generation Failed</div>
                    <div className="text-[11px]">{genError}</div>
                  </div>
                </div>
              )}

              {/* Action Button */}
              <button
                onClick={handleGenerate}
                disabled={isGenerating || !genPrompt.trim()}
                className={`w-full py-2.5 px-4 rounded-lg font-bold text-xs flex items-center justify-center gap-2 transition-all cursor-pointer shadow-sm ${
                  isGenerating || !genPrompt.trim()
                    ? 'bg-slate-200 text-slate-400 cursor-not-allowed'
                    : 'bg-indigo-600 hover:bg-indigo-700 text-white shadow-indigo-600/20'
                }`}
              >
                {isGenerating ? (
                  <>
                    <RefreshCw className="w-4 h-4 animate-spin text-white" />
                    <span>Synthesizing Graphic with Gemini...</span>
                  </>
                ) : (
                  <>
                    <Sparkles className="w-4 h-4 text-indigo-200" />
                    <span>Generate Image</span>
                  </>
                )}
              </button>
            </div>
          ) : (
            /* EDITING PANEL */
            <div className="bg-white rounded-xl border border-slate-200 p-5 shadow-xs space-y-4">
              <div className="flex items-center justify-between">
                <span className="text-xs font-bold text-slate-800 uppercase tracking-wider flex items-center gap-1.5">
                  <Sliders className="w-3.5 h-3.5 text-indigo-600" />
                  Edit Image with Prompt
                </span>
                <span className="text-[11px] text-slate-400 font-mono">Semantic Modification</span>
              </div>

              {/* Source Image Selector */}
              <div>
                <label className="block text-[11px] font-semibold text-slate-700 mb-1.5">
                  1. Source Image to Edit
                </label>
                {sourceImage || sourceSvgCode ? (
                  <div className="relative rounded-lg border border-slate-200 overflow-hidden bg-slate-50 p-2 flex items-center gap-3">
                    <div className="w-16 h-16 rounded-md border border-slate-300 overflow-hidden bg-white flex items-center justify-center shrink-0">
                      <img
                        src={sourceImage || `data:image/svg+xml;base64,${btoa(sourceSvgCode || '')}`}
                        alt="Source preview"
                        className="max-h-full max-w-full object-contain"
                      />
                    </div>
                    <div className="flex-1 min-w-0">
                      <div className="text-xs font-bold text-slate-800 truncate">Source Image Loaded</div>
                      <div className="text-[11px] text-slate-500">
                        {sourceSvgCode ? 'Vector SVG (Prompt-Editable)' : 'Raster Image'}
                      </div>
                    </div>
                    <button
                      onClick={() => {
                        setSourceImage(null);
                        setSourceSvgCode(null);
                      }}
                      className="p-1.5 text-slate-400 hover:text-rose-600 rounded-md hover:bg-rose-50 cursor-pointer"
                      title="Remove image"
                    >
                      <Trash2 className="w-4 h-4" />
                    </button>
                  </div>
                ) : (
                  <div
                    onClick={() => fileInputRef.current?.click()}
                    className="border-2 border-dashed border-slate-300 hover:border-indigo-500 rounded-lg p-5 text-center cursor-pointer transition-colors bg-slate-50/50"
                  >
                    <Upload className="w-6 h-6 text-slate-400 mx-auto mb-1.5" />
                    <span className="text-xs font-bold text-slate-700 block">Click to Upload Image</span>
                    <span className="text-[11px] text-slate-500">SVG, PNG, JPG, or WebP</span>
                    <input
                      ref={fileInputRef}
                      type="file"
                      accept="image/*,.svg"
                      onChange={handleFileUpload}
                      className="hidden"
                    />
                  </div>
                )}
              </div>

              {/* Edit Prompt */}
              <div>
                <label className="block text-[11px] font-semibold text-slate-700 mb-1.5">
                  2. Modification Instruction
                </label>
                <textarea
                  value={editPrompt}
                  onChange={(e) => setEditPrompt(e.target.value)}
                  rows={3}
                  placeholder="Describe changes (e.g. 'Add a green organic label', 'Change packaging to kraft paper')..."
                  className="w-full bg-slate-50 border border-slate-300 rounded-lg p-3 text-xs text-slate-900 focus:outline-none focus:border-indigo-600 focus:bg-white transition-all shadow-inner"
                />
              </div>

              {/* Edit Presets */}
              <div className="space-y-1.5">
                <span className="text-[11px] font-semibold text-slate-600">Quick Edit Presets:</span>
                <div className="flex flex-col gap-1">
                  {editPresets.map((preset, idx) => (
                    <button
                      key={idx}
                      onClick={() => setEditPrompt(preset)}
                      className="text-left px-2 py-1.5 rounded bg-slate-100 hover:bg-indigo-50 hover:text-indigo-700 border border-slate-200 text-[11px] text-slate-700 transition-all cursor-pointer flex items-center justify-between"
                    >
                      <span>{preset}</span>
                      <ArrowRight className="w-3 h-3 opacity-60" />
                    </button>
                  ))}
                </div>
              </div>

              {/* Error Banner */}
              {editError && (
                <div className="p-3 rounded-lg bg-rose-50 border border-rose-200 text-rose-700 text-xs flex items-start gap-2">
                  <AlertCircle className="w-4 h-4 shrink-0 mt-0.5" />
                  <div>
                    <div className="font-bold">Edit Failed</div>
                    <div className="text-[11px]">{editError}</div>
                  </div>
                </div>
              )}

              {/* Action Button */}
              <button
                onClick={handleEdit}
                disabled={isEditing || (!sourceImage && !sourceSvgCode) || !editPrompt.trim()}
                className={`w-full py-2.5 px-4 rounded-lg font-bold text-xs flex items-center justify-center gap-2 transition-all cursor-pointer shadow-sm ${
                  isEditing || (!sourceImage && !sourceSvgCode) || !editPrompt.trim()
                    ? 'bg-slate-200 text-slate-400 cursor-not-allowed'
                    : 'bg-indigo-600 hover:bg-indigo-700 text-white shadow-indigo-600/20'
                }`}
              >
                {isEditing ? (
                  <>
                    <RefreshCw className="w-4 h-4 animate-spin text-white" />
                    <span>Applying Edits with Gemini...</span>
                  </>
                ) : (
                  <>
                    <Wand2 className="w-4 h-4 text-indigo-200" />
                    <span>Apply Image Edits</span>
                  </>
                )}
              </button>
            </div>
          )}

          {/* Architecture Specifications */}
          <div className="bg-slate-50 rounded-xl border border-slate-200 p-4 text-xs text-slate-600 space-y-2">
            <div className="flex items-center gap-2 font-bold text-slate-800">
              <Info className="w-4 h-4 text-indigo-600" />
              <span>Studio Engine Features</span>
            </div>
            <ul className="list-disc list-inside space-y-1 text-[11px] text-slate-600">
              <li>
                <strong className="text-slate-800">Vector Synthesis:</strong> Scalable vector graphics with gradient lighting and drop shadows.
              </li>
              <li>
                <strong className="text-slate-800">Text Prompt Editing:</strong> Inplace semantic modifications (color, labels, badges, layouts).
              </li>
              <li>
                <strong className="text-slate-800">Free Tier Friendly:</strong> Powered natively by <code className="bg-slate-200 px-1 py-0.5 rounded text-[10px]">gemini-3.8-flash</code>.
              </li>
            </ul>
          </div>
        </div>

        {/* Display & Gallery Column (7 cols) */}
        <div className="lg:col-span-7 space-y-4">
          {/* Active Preview Card */}
          <div className="bg-white rounded-xl border border-slate-200 p-5 shadow-xs flex flex-col justify-between min-h-[460px]">
            <div>
              <div className="flex items-center justify-between pb-3 border-b border-slate-100 mb-4 flex-wrap gap-2">
                <span className="text-xs font-bold text-slate-800 uppercase tracking-wider flex items-center gap-2">
                  <ImageIcon className="w-4 h-4 text-indigo-600" />
                  Render Canvas &amp; Output
                </span>
                {activeItem && (
                  <div className="flex items-center gap-2 flex-wrap">
                    <span className="px-2 py-0.5 rounded text-[11px] font-mono bg-indigo-50 text-indigo-700 border border-indigo-200">
                      {activeItem.aspectRatio} • {activeItem.engine}
                    </span>
                    {activeItem.svgCode && (
                      <button
                        onClick={handleCopySvg}
                        className="px-2.5 py-1 rounded bg-slate-100 hover:bg-slate-200 text-slate-700 text-xs font-semibold flex items-center gap-1 transition-all cursor-pointer"
                        title="Copy SVG Code"
                      >
                        {copied ? <Check className="w-3 h-3 text-emerald-600" /> : <Copy className="w-3 h-3" />}
                        <span>{copied ? 'Copied SVG' : 'Copy SVG'}</span>
                      </button>
                    )}
                    <button
                      onClick={() => handleSendToEditor(activeItem)}
                      className="px-2.5 py-1 rounded bg-slate-100 hover:bg-indigo-50 hover:text-indigo-700 text-slate-700 text-xs font-semibold flex items-center gap-1 transition-all cursor-pointer"
                    >
                      <Sliders className="w-3 h-3" />
                      <span>Edit This</span>
                    </button>
                    <button
                      onClick={() =>
                        handleDownload(activeItem.imageUrl, `${activeItem.id}.${activeItem.format === 'svg' ? 'svg' : 'png'}`)
                      }
                      className="px-2.5 py-1 rounded bg-indigo-600 hover:bg-indigo-700 text-white text-xs font-semibold flex items-center gap-1 transition-all cursor-pointer shadow-xs"
                    >
                      <Download className="w-3 h-3" />
                      <span>Download {activeItem.format === 'svg' ? 'SVG' : 'PNG'}</span>
                    </button>
                  </div>
                )}
              </div>

              {/* Main Image Rendering Area */}
              {isGenerating || isEditing ? (
                <div className="flex flex-col items-center justify-center h-[340px] bg-slate-50 rounded-lg border border-slate-200 border-dashed text-slate-500 space-y-3">
                  <div className="w-12 h-12 rounded-full border-4 border-indigo-200 border-t-indigo-600 animate-spin flex items-center justify-center">
                    <Sparkles className="w-5 h-5 text-indigo-600" />
                  </div>
                  <div className="text-xs font-bold text-slate-700">
                    {isGenerating ? 'Synthesizing graphic with Gemini...' : 'Applying text edits to graphic...'}
                  </div>
                  <div className="text-[11px] text-slate-400 max-w-sm text-center">
                    Generating vectors, gradients, and semantic prompt matching...
                  </div>
                </div>
              ) : activeItem ? (
                <div className="space-y-4">
                  {activeItem.type === 'edit' && activeItem.originalImageUrl ? (
                    /* Side-by-side comparison for edits */
                    <div className="grid grid-cols-2 gap-3">
                      <div className="space-y-1">
                        <span className="text-[11px] font-bold text-slate-500 uppercase tracking-wider">
                          Before (Original)
                        </span>
                        <div className="rounded-lg overflow-hidden border border-slate-200 bg-slate-50 aspect-square flex items-center justify-center p-2">
                          <img
                            src={activeItem.originalImageUrl}
                            alt="Original"
                            className="max-h-full max-w-full object-contain"
                          />
                        </div>
                      </div>

                      <div className="space-y-1">
                        <span className="text-[11px] font-bold text-indigo-600 uppercase tracking-wider flex items-center gap-1">
                          <Sparkles className="w-3 h-3" />
                          After (Edited)
                        </span>
                        <div className="rounded-lg overflow-hidden border border-indigo-200 bg-white aspect-square flex items-center justify-center p-2 shadow-sm">
                          <img
                            src={activeItem.imageUrl}
                            alt="Edited"
                            className="max-h-full max-w-full object-contain"
                          />
                        </div>
                      </div>
                    </div>
                  ) : (
                    /* Single image view for generation */
                    <div className="rounded-lg overflow-hidden border border-slate-200 bg-white flex items-center justify-center min-h-[320px] max-h-[440px] p-3 shadow-xs">
                      <img
                        src={activeItem.imageUrl}
                        alt="Rendered output"
                        className="max-h-[400px] max-w-full object-contain"
                      />
                    </div>
                  )}

                  {/* Prompt summary */}
                  <div className="bg-slate-50 border border-slate-200 rounded-lg p-3 text-xs">
                    <span className="font-bold text-slate-700">Prompt: </span>
                    <span className="text-slate-600 font-mono text-[11px]">{activeItem.prompt}</span>
                  </div>
                </div>
              ) : (
                <div className="flex flex-col items-center justify-center h-[340px] bg-slate-50/70 rounded-lg border border-slate-200 border-dashed text-slate-400 space-y-2">
                  <div className="w-12 h-12 rounded-full bg-slate-100 flex items-center justify-center text-slate-400">
                    <ImageIcon className="w-6 h-6" />
                  </div>
                  <div className="text-xs font-bold text-slate-600">No Image Rendered Yet</div>
                  <div className="text-[11px] text-slate-500 text-center max-w-xs">
                    Select a preset or enter a prompt on the left and click "Generate Image" to synthesize.
                  </div>
                </div>
              )}
            </div>

            {/* Gallery Thumbnail Strip */}
            {history.length > 0 && (
              <div className="pt-4 border-t border-slate-100">
                <span className="text-[11px] font-bold text-slate-700 uppercase tracking-wider block mb-2">
                  Session Gallery ({history.length} items)
                </span>
                <div className="flex gap-2 overflow-x-auto pb-1 scrollbar-none">
                  {history.map((item) => (
                    <button
                      key={item.id}
                      onClick={() => setActiveItem(item)}
                      className={`relative w-14 h-14 rounded-lg overflow-hidden border-2 shrink-0 transition-all cursor-pointer bg-white p-0.5 ${
                        activeItem?.id === item.id
                          ? 'border-indigo-600 ring-2 ring-indigo-200'
                          : 'border-slate-200 hover:border-slate-400 opacity-80 hover:opacity-100'
                      }`}
                    >
                      <img
                        src={item.imageUrl}
                        alt="Thumbnail"
                        className="w-full h-full object-contain"
                      />
                      {item.type === 'edit' && (
                        <span className="absolute bottom-0 right-0 bg-indigo-600 text-[8px] font-bold text-white px-1 rounded-tl">
                          EDIT
                        </span>
                      )}
                    </button>
                  ))}
                </div>
              </div>
            )}
          </div>
        </div>
      </div>
    </div>
  );
};
