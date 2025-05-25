# 🖼️ Projet C - Traitement d'images BMP (EFREI)

## 📌 Objectif
Projet réalisé en C dans le cadre du module **TI202** à l’EFREI.  
Il permet de **charger, traiter et sauvegarder** des images BMP **8 bits** (niveaux de gris) et **24 bits** (couleur).

---

## 🛠️ Fonctionnalités

### 📥 Chargement / Sauvegarde
- BMP 8 bits (palette de couleurs)
- BMP 24 bits (couleur vraie)
- Gestion automatique du format (via menu)

### 🧮 Traitements disponibles

#### 📸 Images 8 bits
- Inversion (négatif)
- Modification de la luminosité
- Seuillage (binarisation)
- Filtres de convolution :
   - Flou (box blur, gaussien)
   - Détection de contours
   - Relief (emboss)
   - Netteté (sharpen)

#### 🌈 Images 24 bits
- Inversion (négatif)
- Conversion en niveaux de gris
- Modification de la luminosité
- Filtres de convolution :
   - Flou (box blur, gaussien)
   - Contours
   - Relief
   - Netteté

---

## 🧩 Structure du projet
ProjetC/
- bmp8.h / bmp8.c // Fonctions pour images 8 bits
- bmp24.h / bmp24.c // Fonctions pour images 24 bits
- histogram.h / histogram.c // Égalisation 
- main.c // Interface console (menus, tests)
- CMakeLists.txt // Compilation CLion / CMake
- README.md // Documentation


## ▶️ Compilation (avec CMake)

### ✅ CLion (recommandé)
- Ouvrir le dossier dans CLion
- Vérifier que tous les fichiers `.c` sont inclus dans `CMakeLists.txt`
- Cliquer sur **Build > Rebuild Project**

### 🔧 Ligne de commande :
```bash
mkdir build
cd build
cmake ..
make
./ProjetC
