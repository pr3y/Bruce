import sys  
with open('platformio.ini', 'r') as f:  
    lines = f.readlines()  
ECHO est  ativado.
filtered_lines = [line for line in lines if line.strip() != 'lib_deps =']  
ECHO est  ativado.
with open('platformio.ini', 'w') as f:  
    f.writelines(filtered_lines)  
ECHO est  ativado.
print(f'Removed {len(lines) - len(filtered_lines)} empty lib_deps lines')  
