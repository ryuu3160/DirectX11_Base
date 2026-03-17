import os
import re
import argparse
from pathlib import Path

def extract_comment(lines, class_line_index, class_name):
    """クラス定義の直前のコメントを抽出"""
    description = ""
    in_description_block = False
    description_lines = []

    for i in range(class_line_index - 1, max(0, class_line_index - 30), -1):
        line = lines[i].strip()

        if line == "// --------------------------":
            in_description_block = True
            continue

        if line == "// ---ComponentDescription---":
            if description_lines:
                description_lines.reverse()
                description = " ".join(line for line in description_lines if line)
            break

        if in_description_block:
            if line.startswith("//"):
                comment = line[2:].strip()
                description_lines.append(comment)
            elif not line:
                continue
            else:
                break
        elif line and not line.startswith("//") and not line.startswith("*") and not line.startswith("/*"):
            break

    if not description:
        description = class_name + " component"

    return description.strip()

def guess_category(filepath):
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
    if exclude_dirs is None:
        exclude_dirs = ['ThirdParty', 'External', 'Vendor', 'Build', 'Generated']

    component_classes = []
    source_path = Path(source_dir)

    print(f"Scanning directory: {source_dir}")

    for hpp_file in source_path.rglob('*.hpp'):
        if any(exclude_dir in str(hpp_file) for exclude_dir in exclude_dirs):
            continue

        try:
            with open(hpp_file, 'r', encoding='shift-jis', errors='ignore') as f:
                lines = f.readlines()
                content = ''.join(lines)

                # ENGINE_API マクロがクラス宣言の前にある場合も考慮
                if('ENGINE_API' in content):
                    content = content.replace('ENGINE_API', '')

                # "class XXX : public Component"を検索
                # 複数行にまたがる可能性も考慮
                pattern = r'class\s+(\w+)\s*:\s*public\s+(Component|RenderComponent|ColliderBase)'
                matches = re.finditer(pattern, content)

                for match in matches:
                    class_name = match.group(1)

                    # Component 自体はスキップ
                    if class_name == 'Component':
                        continue
                    # Transformもスキップ
                    if class_name == 'Transform':
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

                    class_line_index = content[:match.start()].count('\n')

                    description = extract_comment(lines, class_line_index, class_name)
                    if not description:
                        description = f"{class_name} component"

                    category = guess_category(str(hpp_file))
                    relative_path = hpp_file.relative_to(source_path)

                    component_info = {
                        'name': class_name,
                        'category': category,
                        'description': description,
                        'header': str(relative_path).replace('\\', '/')
                    }

                    if not any(c['name'] == class_name for c in component_classes):
                        component_classes.append(component_info)
                        print(f"  Found: {class_name} ({category})")

        except Exception as e:
            print(f"  Warning: Failed to parse {hpp_file}: {e}")

    return component_classes

def generate_cpp(component_classes, output_path, function_name, registry_mode, export_api, registry_param_name, default_extension):
    """
    registry_mode:
      - 'singleton' : ComponentRegistry::GetInstance() を使う
      - 'param'     : 引数 registry_param_name を使う (ComponentRegistry*)
      - 'callback'  : 引数として void(*Register)(const ComponentInfo&) を受け取って Register(info) を呼ぶ
    export_api:
      - True  : extern "C" __declspec(dllexport)
      - False : 通常関数
    """

    includes = set()
    for comp in component_classes:
        includes.add(f'#include "{comp["header"]}"')

    output_path = Path(output_path)
    output_path.parent.mkdir(parents=True, exist_ok=True)

    # 関数シグネチャ
    if registry_mode == "param":
        signature = f"void {function_name}(ComponentRegistry* {registry_param_name})"
    elif registry_mode == "callback":
        signature = f"void {function_name}(void(*Register)(const ComponentInfo&))"
    else:
        signature = f"void {function_name}()"

    if export_api:
        signature = f'extern "C" __declspec(dllexport) {signature}'

    # 登録先
    if registry_mode == "param":
        register_line = f"{registry_param_name}->RegisterComponent(info);"
    elif registry_mode == "callback":
        register_line = "Register(info);"
    else:
        register_line = "ComponentRegistry::GetInstance().RegisterComponent(info);"

    code = f'''// ComponentRegistry_Generated.cpp
// このファイルは自動生成されています。編集しないでください。
// Generated by Scripts/GenerateComponentRegistry.py

#include "Engine/Core/System/Component/ComponentRegistry.hpp"
#include "Engine/Core/System/Object/GameObject.hpp"

// Component headers
{chr(10).join(sorted(includes))}

{signature}
{{
'''

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
        {register_line}
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
    print(f"Function: {function_name}")
    print(f"RegistryMode: {registry_mode}")
    print(f"Export: {export_api}")

def main():
    parser = argparse.ArgumentParser(description='Generate component registry')
    parser.add_argument('--source-dir', required=True, help='Source directory to scan')
    parser.add_argument('--output', required=True, help='Output file path')
    parser.add_argument('--exclude', nargs='*', help='Directories to exclude')

    parser.add_argument('--function-name', default='RegisterAllComponents', help='Generated function name')
    parser.add_argument('--registry-mode', choices=['singleton', 'param', 'callback'], default='singleton', help='How to access ComponentRegistry')
    parser.add_argument('--export', action='store_true', help='Export the generated function (extern "C" __declspec(dllexport))')
    parser.add_argument('--registry-param-name', default='registry', help='Parameter name used when registry-mode=param')

    args = parser.parse_args()

    print("=" * 60)
    print("Component Registry Generator")
    print("=" * 60)

    components = find_component_classes(args.source_dir, args.exclude)

    if not components:
        print("\n Warning: No components found!")
        with open(args.output, 'w', encoding='shift-jis') as f:
            f.write('// No components found\n')
        return

    generate_cpp(
        components,
        args.output,
        function_name=args.function_name,
        registry_mode=args.registry_mode,
        export_api=args.export,
        registry_param_name=args.registry_param_name,
        default_extension=None
    )

    print("\n" + "=" * 60)
    print("Component Registry Generation Complete!")
    print("=" * 60)

if __name__ == '__main__':
    main()