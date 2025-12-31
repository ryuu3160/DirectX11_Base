param (
    [string]$vcxprojPath,
    [string]$includeDir
)

[xml]$proj = Get-Content $vcxprojPath -Raw -Encoding UTF8

$debugCond   = "'`$(Configuration)|`$(Platform)'=='Debug|x64'"
$releaseCond = "'`$(Configuration)|`$(Platform)'=='Release|x64'"

$namespace = $proj.Project.NamespaceURI
$xmlNsMgr = New-Object System.Xml.XmlNamespaceManager($proj.NameTable)
$xmlNsMgr.AddNamespace("ns", $namespace)

foreach ($group in $proj.Project.ItemDefinitionGroup)
{
    if ($group.Condition -eq $debugCond -or $group.Condition -eq $releaseCond)
    {
        $clcompile = $group.ClCompile
        if ($clcompile)
        {
            # 名前空間付きでノード取得
            $dirsNodes = $clcompile.SelectNodes("ns:AdditionalIncludeDirectories", $xmlNsMgr)
            if ($dirsNodes.Count -gt 0)
            {
                $dirsNode = $dirsNodes[0]
                $currentValue = $dirsNode.InnerText

                if ($currentValue -notlike "*$includeDir*")
                {
                    if ($currentValue -match ';%(AdditionalIncludeDirectories)')
                    {
                        $newValue = $currentValue -replace ';%(AdditionalIncludeDirectories)', ";$includeDir;%(AdditionalIncludeDirectories)"
                    }
                    else
                    {
                        $newValue = "$currentValue;$includeDir"
                    }
                    $dirsNode.InnerText = $newValue
                }
                # 2個目以降は削除
                for ($i = 1; $i -lt $dirsNodes.Count; $i++)
                {
                    $clcompile.RemoveChild($dirsNodes[$i])
                }
            }
            else
            {
                # ノードが無ければ名前空間付きで新規作成
                $newNode = $proj.CreateElement("AdditionalIncludeDirectories", $namespace)
                $newNode.InnerText = $includeDir
                $clcompile.AppendChild($newNode)
            }
        }
    }
}
$proj.Save($vcxprojPath)