# Week 1

## Opengl documentation
[glut doc](https://www.opengl.org/resources/libraries/glut/spec3/node1.html)

## Opengl installation
### Gerekli Paketleri Kurma
```bash
sudo apt update
sudo apt upgrade
sudo apt install build-essential
sudo apt install freeglut3 freeglut3-dev
sudo apt install libgl1-mesa-dev
sudo apt install libglu1-mesa-dev
sudo apt install mesa-utils
```

### OpenGL Kurulumunu Test Etme
Kurulumun doğru yapıldığını kontrol etmek için:

```bash
glxinfo | grep "OpenGL version"
```

Eğer OpenGL sürümü görünüyorsa kurulum başarılıdır.

İstersen basit bir test animasyonu çalıştır:

```bash
glxgears
```

Dönen dişliler görüyorsan sistem hazır ✅
