#!/usr/bin/env python3
import sys
import json
import math
import collections
import os

def compute_fallback_embeddings(texts, dim=384):
    """
    Highly robust, self-contained fallback vectorizer.
    Uses character N-grams (n=3,4,5) and projects them deterministically
    via FNV-1a hashing into a dense 384-dimensional vector, normalized to L2 unit length.
    """
    embeddings = []
    # 1. First pass: compute TF and document frequencies for IDF
    doc_freqs = collections.defaultdict(int)
    doc_tfs = []
    
    for text in texts:
        # Extract char n-grams
        ngrams = []
        for n in (3, 4, 5):
            for i in range(len(text) - n + 1):
                ngrams.append(text[i:i+n])
        
        # Lowercase n-grams if standard characters
        ngrams = [g.lower() for g in ngrams]
        if not ngrams:
            ngrams = [text.lower()] if text else ["_empty_"]
            
        tf = collections.defaultdict(int)
        for g in ngrams:
            tf[g] += 1
            
        doc_tfs.append(tf)
        for g in tf:
            doc_freqs[g] += 1
            
    num_docs = len(texts)
    
    # 2. Second pass: build L2 normalized hash-projected TF-IDF vectors
    for tf in doc_tfs:
        vec = [0.0] * dim
        for gram, count in tf.items():
            # TF weighting: 1 + log(tf)
            tf_weight = 1.0 + math.log(count)
            # IDF weighting: log(1 + num_docs / doc_freq)
            idf = math.log(1.0 + (num_docs / (1.0 + doc_freqs[gram])))
            weight = tf_weight * idf
            
            # Deterministic FNV-1a hash projection to project gram to [0, dim)
            # FNV-1a 32-bit hash params
            h = 2166136261
            for char in gram:
                h = h ^ ord(char)
                h = (h * 16777619) & 0xFFFFFFFF
            idx = h % dim
            vec[idx] += weight
            
        # Compute L2 norm
        norm = math.sqrt(sum(val * val for val in vec))
        if norm > 1e-9:
            vec = [val / norm for val in vec]
        else:
            # Fallback for empty strings
            vec = [0.0] * dim
            vec[0] = 1.0
            
        embeddings.append(vec)
        
    return embeddings

def main():
    try:
        # Read JSON list of texts from standard input
        input_data = sys.stdin.read().strip()
        if not input_data:
            print(json.dumps([]))
            return
            
        data = json.loads(input_data)
        if isinstance(data, dict):
            prompt = data.get("prompt", "")
            items = data.get("items", [])
            texts = [prompt] + [item.get("path", "") for item in items]
            sizes = [0] + [item.get("size", 0) for item in items]
        else:
            texts = data
            sizes = [0] * len(texts)
            prompt = texts[0] if len(texts) > 0 else ""
            
        # Parse semantic prompt instruction rules
        if len(texts) > 0:
            prompt_lower = prompt.lower()
            is_first_letter = any(kw in prompt_lower for kw in ["first letter", "starting letter", "starts with the same letter", "first char", "starting char"])
            is_extension = any(kw in prompt_lower for kw in ["extension", "file type", "same extension", "file suffix"])
            is_parent_folder = any(kw in prompt_lower for kw in ["parent folder", "same folder", "directory", "same directory"])
            is_size = any(kw in prompt_lower for kw in ["size", "file size", "similar size", "similar file size"])
            
            if is_first_letter or is_extension or is_parent_folder or is_size:
                dim = 384
                embeddings = []
                # Prompt embedding is all zeroes to avoid trigger boosts
                embeddings.append([0.0] * dim)
                
                val_to_dim = {}
                next_dim = 0
                for i in range(1, len(texts)):
                    text = texts[i]
                    size = sizes[i]
                    filename = os.path.basename(text)
                    
                    if is_first_letter:
                        val = filename[0].lower() if filename else ""
                    elif is_extension:
                        val = os.path.splitext(filename)[1].lower() if filename else ""
                    elif is_parent_folder:
                        val = os.path.dirname(text).lower() if text else ""
                    elif is_size:
                        val = int(math.log2(size)) if size > 0 else 0
                    
                    if val not in val_to_dim:
                        val_to_dim[val] = next_dim % dim
                        next_dim += 1
                    
                    vec = [0.0] * dim
                    vec[val_to_dim[val]] = 1.0
                    embeddings.append(vec)
                
                print(json.dumps(embeddings))
                return
            
        # Try to load sentence-transformers
        try:
            from sentence_transformers import SentenceTransformer
            # Load the highly efficient, lightweight 384-dimensional MiniLM model
            model = SentenceTransformer('all-MiniLM-L6-v2')
            
            # Compute embeddings
            raw_embeddings = model.encode(texts, convert_to_numpy=True).tolist()
            
            # Normalize embeddings to L2 unit norm
            normalized_embeddings = []
            for vec in raw_embeddings:
                norm = math.sqrt(sum(val * val for val in vec))
                if norm > 1e-9:
                    normalized_embeddings.append([val / norm for val in vec])
                else:
                    normalized_embeddings.append(vec)
                    
            print(json.dumps(normalized_embeddings))
            
        except ImportError:
            sys.stderr.write("=========================================================================\n")
            sys.stderr.write("            BTTB DEEP LEARNING MINILM SEMANTIC EXTENSION TUTORIAL        \n")
            sys.stderr.write("=========================================================================\n")
            sys.stderr.write("The 'sentence-transformers' Python package is required for full neural  \n")
            sys.stderr.write("MiniLM-L6-v2 contextual embeddings, but it is not currently installed.\n")
            sys.stderr.write("To properly install this preferred deep learning extension, follow these\n")
            sys.stderr.write("simple step-by-step instructions:\n\n")
            sys.stderr.write("STEP 1: Ensure Python 3 and Pip are installed on your system:\n")
            sys.stderr.write("   - On Ubuntu/Debian: run 'sudo apt install python3 python3-pip python3-venv'\n")
            sys.stderr.write("   - On Windows: download & run the installer from https://www.python.org/\n\n")
            sys.stderr.write("STEP 2: Install sentence-transformers via pip (Choose Option A or B):\n")
            sys.stderr.write("   OPTION A (Global/User-level installation - Recommended for simplicity):\n")
            sys.stderr.write("      pip install sentence-transformers\n")
            sys.stderr.write("      (or 'python3 -m pip install sentence-transformers')\n\n")
            sys.stderr.write("   OPTION B (Virtual environment - Recommended for isolation):\n")
            sys.stderr.write("      python3 -m venv bttb_env\n")
            sys.stderr.write("      source bttb_env/bin/activate  # (Windows: bttb_env\\Scripts\\activate)\n")
            sys.stderr.write("      pip install sentence-transformers\n\n")
            sys.stderr.write("STEP 3: Restart Burn to the Brim. It will now automatically load MiniLM!\n")
            sys.stderr.write("=========================================================================\n")
            sys.stderr.write("-> Falling back to L2-normalized deterministic N-gram TF-IDF projection...\n")
            
            embeddings = compute_fallback_embeddings(texts)
            print(json.dumps(embeddings))
            
    except Exception as e:
        sys.stderr.write(f"Embedding generation failed: {str(e)}\n")
        sys.exit(1)

if __name__ == "__main__":
    main()
