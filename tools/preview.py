"""生成离线屏幕预览；使用与固件相同的角色、动作定义和布局，不连接任何服务。"""
import argparse,base64,json
from pathlib import Path
def main():
    parser=argparse.ArgumentParser(description=__doc__);parser.add_argument('--output',type=Path,required=True);args=parser.parse_args()
    root=Path(__file__).resolve().parents[1];assets=root/'main/teloa/assets'
    poses={name:'data:image/png;base64,'+base64.b64encode((assets/(name+'.png')).read_bytes()).decode() for name in ('standing','wink','curious','thinking','reading','celebrating')}
    html=(root/'tools/preview.html').read_text().replace('__POSES__',json.dumps(poses)).replace('__MOTION__',(root/'main/teloa/motion.json').read_text()).replace('__SCRIPT__',(root/'tools/preview.js').read_text())
    with args.output.open('x',encoding='utf-8') as output:output.write(html)
if __name__=='__main__':main()
