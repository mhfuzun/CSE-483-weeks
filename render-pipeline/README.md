## esantial tools
linux mint:
```bash
sudo apt updates
sudo apt install libsdl2-dev libsdl2-ttf-dev pkg-config build-essential
```

macOS:
```bash
brew update
brew install sdl2 pkg-config
brew install libsdl2-ttf-dev
```

Evet, **perspective projection hesabında asıl dönüştürülen şey `position`dır**. Ama “UV ve normal tamamen sabit kalır mı?” sorusunda küçük bir ayrım var:

```text
position  -> model, view, projection, perspective divide, viewport
uv        -> değişmez, sadece taşınır ve interpolate edilir
normal    -> projection'dan etkilenmez ama model/view uzayına dönüştürülür
```

---

## 1. Vertex position için

Vertex pozisyonu şu aşamalardan geçer:

```c
worldPos = Model * vec4(localPos, 1.0);
viewPos  = View  * worldPos;
clipPos  = Projection * viewPos;
```

Sonra perspective divide yapılır:

```c
ndc.x = clipPos.x / clipPos.w;
ndc.y = clipPos.y / clipPos.w;
ndc.z = clipPos.z / clipPos.w;
```

Sonra viewport:

```c
screen.x = (ndc.x * 0.5 + 0.5) * width;
screen.y = (1.0 - (ndc.y * 0.5 + 0.5)) * height;
```

Yani perspektif, ekran konumunu belirlemek için `position` üzerinde çalışır.

---

## 2. UV için

UV, perspektif matrisiyle çarpılmaz.

Yani şunu yapmazsın:

```c
uv = Projection * uv; // yanlış
```

UV sadece vertex ile beraber taşınır:

```c
vertex.uv = obj_uv;
```

Ama piksel içinde UV bulurken **perspective-correct interpolation** yapılır.

Yani vertex aşamasında UV sabit kalır, ama rasterization sırasında piksel için UV şöyle hesaplanır:

```c
uv_pixel =
    (l0 * uv0 / w0 +
     l1 * uv1 / w1 +
     l2 * uv2 / w2)
    /
    (l0 / w0 +
     l1 / w1 +
     l2 / w2);
```

C-benzeri:

```c
float invW =
    l0 * v0.invW +
    l1 * v1.invW +
    l2 * v2.invW;

vec2 uv =
    (l0 * v0.uv * v0.invW +
     l1 * v1.uv * v1.invW +
     l2 * v2.uv * v2.invW) / invW;
```

Buradaki `w`, projection sonrası oluşan `clipPos.w` değeridir.

Yani kısa cevap:

```text
UV projection ile değişmez.
Ama ekrandaki piksel için UV lineer değil, perspective-correct interpolate edilir.
```

---

## 3. Normal için

Normal de projection matrisiyle çarpılmaz.

Ama normal **model transform**dan etkilenebilir. Mesela obje dönerse normal de dönmelidir. Obje non-uniform scale alırsa normal daha özel şekilde dönüştürülmelidir.

Basit durumda:

```c
worldNormal = normalize(mat3(Model) * localNormal);
```

Ama doğru genel yöntem:

```c
normalMatrix = transpose(inverse(mat3(Model)));
worldNormal = normalize(normalMatrix * localNormal);
```

Eğer lighting’i view space’te yapıyorsan:

```c
normalMatrix = transpose(inverse(mat3(View * Model)));
viewNormal = normalize(normalMatrix * localNormal);
```

Ama şunu yapmazsın:

```c
normal = Projection * normal; // yanlış
```

Çünkü projection doğrusal geometrik yönleri fiziksel yüzey normal’i olarak korumaz. Projection sadece ekrana düşürme içindir.

---

## 4. Normal de perspective-correct interpolate edilir mi?

Evet, rasterization sırasında normal de vertex attribute olarak interpolate edilir.

Basit ama perspektifte hatalı yöntem:

```c
N = l0 * N0 + l1 * N1 + l2 * N2;
N = normalize(N);
```

Daha doğru yöntem:

```c
N =
    (l0 * N0 * invW0 +
     l1 * N1 * invW1 +
     l2 * N2 * invW2)
    /
    (l0 * invW0 +
     l1 * invW1 +
     l2 * invW2);

N = normalize(N);
```

Yani normal için de mantık UV ile aynı:

```text
Projection normal'i dönüştürmez.
Ama ekran üzerindeki piksel normal'i perspective-correct interpolate edilir.
Sonra tekrar normalize edilir.
```

---

## 5. En doğru pipeline

Senin software rasterizer için düşünürsek:

```c
// Vertex stage
v.worldPos = Model * vec4(localPos, 1.0);
v.normal   = normalize(transpose(inverse(mat3(Model))) * localNormal);
v.uv       = objUV;

v.clipPos  = Projection * View * v.worldPos;
v.invW     = 1.0 / v.clipPos.w;
```

Clipping sırasında:

```c
newVertex.clipPos  = lerp(a.clipPos,  b.clipPos,  t);
newVertex.worldPos = lerp(a.worldPos, b.worldPos, t);
newVertex.normal   = lerp(a.normal,   b.normal,   t);
newVertex.uv       = lerp(a.uv,       b.uv,       t);
```

Perspective divide ve viewport:

```c
ndc = clipPos.xyz / clipPos.w;
screen = viewport(ndc);
```

Rasterization sırasında:

```c
uv = perspective_correct(uv0, uv1, uv2);
N  = perspective_correct(N0, N1, N2);
P  = perspective_correct(worldPos0, worldPos1, worldPos2);

N = normalize(N);
```

Sonra:

```c
texColor = sample(texture, uv);

L = normalize(lightPos - P);
V = normalize(cameraPos - P);
H = normalize(L + V);

color = lighting(texColor, N, L, V, H);
```

---

Kısaca:

```text
Perspective projection:
    position için uygulanır.

UV:
    projection ile değişmez;
    pikselde perspective-correct interpolate edilir.

Normal:
    projection ile değişmez;
    model/view uzayına normal matrix ile dönüştürülür;
    pikselde perspective-correct interpolate edilir ve normalize edilir.
```
