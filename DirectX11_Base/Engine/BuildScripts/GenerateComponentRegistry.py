import os
import re
import argparse
from pathlib import Path

def extract_comment(lines, class_line_index, class_name):
    """クラス定義の直前のコメントを抽出"""
    description = ""
    in_description_block = False
    description_lines = []
    
    # クラス定義の前の行から上に向かって探す
    for i in range(class_line_index - 1, max(0, class_line_index - 30), -1):
        line = lines[i].strip()
        
        # 終了マーカーを検出
        if line == "// --------------------------":
            in_description_block = True
            continue
        
        # 開始マーカーを検出
        if line == "// ---ComponentDescription---":
            # 説明が見つかった
            if description_lines:
                # 逆順になっているので反転
                description_lines.reverse()
                # 空行を除去して結合
                description = " ".join(line for line in description_lines if line)
            break
        
        # ブロック内のコメントを収集
        if in_description_block:
            if line.startswith("//"):
                comment = line[2:].strip()
                description_lines.append(comment)
            elif not line:
                # 空行は無視
                continue
            else:
                # コメント以外が出てきたらブロック外
                break
        
        # ブロック探索中にコメント以外が出てきたら終了
        elif line and not line.startswith("//") and not line.startswith("*") and not line.startswith("/*"):
            break
    
    # 説明が見つからなかった場合はコンポーネント名を返す
    if not description:
        description = class_name + " component"
    
    return description.strip()

def guess_category(filepath):
    """ファイルパスからカテゴリを推測"""
    filepath_lower = filepath.lower()
    
    if 'render' in filepath_lower or 'camera' in filepath_lower or 'light' in filepath_lower:
        return 'Rendering'
    elif 'physics' in filepath_lower or 'rigidbody' in filepath_lower:
        return 'Physics'
    elif 'collider' in filepath_lower:
        return 'Collider'
    elif 'audio' in filepath_lower or 'sound' in filepath_lower:
        return 'Audio'
    elif 'particle' in filepath_lower or 'effect' in filepath_lower:
        return 'Effects'
    elif 'anim' in filepath_lower:
        return 'Animation'
    elif 'ui' in filepath_lower or 'text' in filepath_lower or 'image' in filepath_lower:
        return 'UI'
    else:
        return 'General'

def find_component_classes(source_dir, exclude_dirs=None):
    """Component を継承しているクラスを検出"""
    
    if exclude_dirs is None:
        exclude_dirs = ['ThirdParty', 'External', 'Vendor', 'Build', 'Generated']
    
    component_classes = []
    source_path = Path(source_dir)
    
    print(f"Scanning directory: {source_dir}")
    
    # すべての .hpp ファイルをスキャン
    for hpp_file in source_path.rglob('*.hpp'):
        # 除外ディレクトリをスキップ
        if any(exclude_dir in str(hpp_file) for exclude_dir in exclude_dirs):
            continue
        
        try:
            with open(hpp_file, 'r', encoding='shift-jis', errors='ignore') as f:
                lines = f.readlines()
                content = ''.join(lines)
                
                # "class XXX : public Component" を検出
                # 複数行にまたがる可能性も考慮
                pattern = r'class\s+(\w+)\s*:\s*public\s+(Component|RenderComponent|ColliderBase)'
                matches = re.finditer(pattern, content)
                
                for match in matches:
                    class_name = match.group(1)
                    
                    # Component 自体はスキップ
                    if class_name == 'Component':
                        continue
                    # RenderComponent もスキップ
                    if class_name == 'RenderComponent':
                        continue
                    # ColliderBase もスキップ
                    if class_name == 'ColliderBase':
                        continue
                    # GridComponent もスキップ
                    if class_name == 'GridComponent':
                        continue
                    
                    # クラス定義の行番号を取得
                    class_line_index = content[:match.start()].count('\n')
                    
                    # コメントから説明を抽出
                    description = extract_comment(lines, class_line_index,class_name)
                    if not description:
                        description = f"{class_name} component"
                    
                    # カテゴリを推測
                    category = guess_category(str(hpp_file))
                    
                    # 相対パス
                    relative_path = hpp_file.relative_to(source_path)
                    
                    component_info = {
                        'name': class_name,
                        'category': category,
                        'description': description,
                        'header': str(relative_path).replace('\\', '/')
                    }
                    
                    # 重複チェック
                    if not any(c['name'] == class_name for c in component_classes):
                        component_classes.append(component_info)
                        print(f"  Found: {class_name} ({category})")
        
        except Exception as e:
            print(f"  Warning: Failed to parse {hpp_file}: {e}")
    
    return component_classes

def generate_cpp(component_classes, output_path):
    """登録用の C++ コードを生成（Shift-JIS）"""
    
    # ヘッダーインクルード
    includes = set()
    for comp in component_classes:
        includes.add(f'#include "{comp["header"]}"')
    
    # 関数ベースの実装に変更
    code = f'''// ComponentRegistry_Generated.cpp
// このファイルは自動生成されています。編集しないでください。
// Generated by Scripts/GenerateComponentRegistry.py

#include "Engine/Core/System/Component/ComponentRegistry.hpp"
#include "Engine/Core/System/Object/GameObject.hpp"

// Component headers
{chr(10).join(sorted(includes))}

// 初期化関数として実装
void RegisterAllComponents()
{{
'''
    
    # 各コンポーネントの登録コード
    for comp in component_classes:
        description = comp['description'].replace('\\', '\\\\').replace('"', '\\"')
        
        code += f'''
    // {comp['name']}
    {{
        ComponentInfo info;
        info.Name = "{comp['name']}";
        info.Category = "{comp['category']}";
        info.Description = "{description}";
        info.CreateFunc = [](GameObject* owner) -> Component* {{
            return owner->AddComponent<{comp['name']}>();
        }};
        ComponentRegistry::GetInstance().RegisterComponent(info);
    }}
'''
    
    code += '''
}
'''
    
    # ファイルに書き込み
    output_path = Path(output_path)
    output_path.parent.mkdir(parents=True, exist_ok=True)
    
    with open(output_path, 'w', encoding='shift-jis') as f:
        f.write(code)
    
    print(f"\nGenerated: {output_path}")
    print(f"Registered {len(component_classes)} components")

def main():
    parser = argparse.ArgumentParser(description='Generate component registry')
    parser.add_argument('--source-dir', required=True, help='Source directory to scan')
    parser.add_argument('--output', required=True, help='Output file path')
    parser.add_argument('--exclude', nargs='*', help='Directories to exclude')
    
    args = parser.parse_args()
    
    print("=" * 60)
    print("Component Registry Generator")
    print("=" * 60)
    
    # Component クラスを検出
    components = find_component_classes(args.source_dir, args.exclude)
    
    if not components:
        print("\n Warning: No components found!")
        # 空のファイルを生成
        with open(args.output, 'w', encoding='shift-jis') as f:
            f.write('// No components found\n')
        return
    
    # C++ コードを生成
    generate_cpp(components, args.output)
    
    print("\n" + "=" * 60)
    print("Component Registry Generation Complete!")
    print("=" * 60)

if __name__ == '__main__':
    main()