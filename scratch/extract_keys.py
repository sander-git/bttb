import re
import os
import glob
import json

def main():
    # Regular expression to match _T("key", "value") where value can have escaped characters and span across lines or have concatenated quotes.
    # To be safe and simple, let's find _T("key", "value") matches.
    pattern = re.compile(r'_T\s*\(\s*"([^"]+)"\s*,\s*"((?:[^"\\]|\\.)*)"\s*\)')
    
    src_dir = "/home/sander/src/antigravity/project1/bttb_cpp/src"
    files = glob.glob(os.path.join(src_dir, "*.cpp")) + glob.glob(os.path.join(src_dir, "*.hpp"))
    
    extracted = {}
    for filepath in files:
        with open(filepath, 'r', encoding='utf-8', errors='ignore') as f:
            content = f.read()
            for match in pattern.finditer(content):
                key, val = match.groups()
                # Unescape string
                # We can replace standard escapes
                val_unescaped = val.replace('\\n', '\n').replace('\\r', '\r').replace('\\t', '\t').replace('\\"', '"').replace('\\\\', '\\')
                extracted[key] = val_unescaped

    with open("/home/sander/src/antigravity/project1/bttb_cpp/scratch/keys.json", "w", encoding="utf-8") as f:
        json.dump(extracted, f, indent=4, ensure_ascii=False)

if __name__ == '__main__':
    main()
