"""把工作台已定稿小松鼠转换为固定 LVGL 像素资源；不生成或改画角色。"""
import hashlib
import json
from pathlib import Path
from PIL import Image

root=Path(__file__).resolve().parents[1]
source=root/'artwork/companion'
target=root/'main/teloa/assets'
target.mkdir(exist_ok=True)
manifest={'schema':1,'width':160,'height':160,'format':'ARGB8888-little-endian','source':'artwork/companion','poses':{}}
for pose in ['standing','wink','curious','thinking','reading','celebrating']:
    original=(source/(pose+'.webp')).read_bytes()
    image=Image.open(source/(pose+'.webp')).convert('RGBA').resize((160,160),Image.Resampling.LANCZOS)
    data=image.tobytes('raw','BGRA');(target/(pose+'.rgba')).write_bytes(data)
    image.save(target/(pose+'.png'),optimize=True)
    manifest['poses'][pose]={'sourceSha256':hashlib.sha256(original).hexdigest(),'rgbaSha256':hashlib.sha256(data).hexdigest()}
(target/'manifest.json').write_text(json.dumps(manifest,ensure_ascii=False,indent=2)+'\n')
