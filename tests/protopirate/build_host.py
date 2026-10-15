from pathlib import Path
import subprocess, concurrent.futures, sys
root=Path(__file__).resolve().parents[2]
src=root/'lib/ProtoPirate/src'
build=root/'tests/protopirate/build';build.mkdir(exist_ok=True)
flags=['gcc','-std=gnu11','-g','-O1','-fsanitize=address,undefined','-fno-omit-frame-pointer','-Wall','-Werror=implicit-function-declaration','-Werror=incompatible-pointer-types','-I'+str(src)]
files=list(src.rglob('*.c'))
def compile(p):
    out=build/(str(p.relative_to(src)).replace('/','_')+'.o')
    run=subprocess.run(flags+['-c',str(p),'-o',str(out)],capture_output=True,text=True)
    if run.returncode:print(run.stderr);raise RuntimeError(str(p))
    return str(out)
with concurrent.futures.ThreadPoolExecutor(max_workers=5) as pool:objs=list(pool.map(compile,files))
subprocess.run(flags+[str(Path(__file__).with_name('native_cli.c'))]+objs+['-o',str(build/'pp_cli')],check=True)
print('Compiled',len(files),'C sources; ASan + UBSan enabled')
