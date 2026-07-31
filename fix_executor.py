import re

with open('Src/shell/Executor.cpp', 'r') as f:
    content = f.read()

# Update signature
content = content.replace(
    'std::string Executor::resolveVirtualPath(const std::string& arg) const {',
    'std::string Executor::resolveVirtualPath(const std::string& arg, const std::string& cwd) {'
)
content = content.replace(
    'std::string base = (arg.empty() || arg[0] != \'/\') ? prompt.getCurrentDir() : "/";',
    'std::string base = (arg.empty() || arg[0] != \'/\') ? cwd : "/";'
)
content = content.replace(
    'std::string Executor::mapToHostPath(const std::string& virtualPath) const {',
    'std::string Executor::mapToHostPath(const std::string& virtualPath) {'
)

# Update calls
content = re.sub(
    r'resolveVirtualPath\(([^)]+)\)',
    r'resolveVirtualPath(\1, prompt.getCurrentDir())',
    content
)

with open('Src/shell/Executor.cpp', 'w') as f:
    f.write(content)
