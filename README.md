<h1 align="center">
  <br>
  <img src="https://i.imgur.com/W85GUpg.png" alt="Prism" width="200">
</h1>

# Prism

Prism is an efficient OCR written in C.

### Built With

* [GNU Make](https://www.gnu.org/software/make/)

## Getting Started

### Prerequisites

* [GNU Make](https://www.gnu.org/software/make/)
```bash
sudo apt update
sudo apt install make
```

* [GTK](https://www.gtk.org/)
```bash
sudo apt install libgtk-3-dev
```

### Installation

1. Clone the repository.

```bash
git clone git@github.com:prismocr/ocr.git
```

2. Compile the project.

```bash
cd ocr
make
```

## Usage

```bash
./build/release/ocr [options]
```
Possible options :
* sharpen [image_path]
* blur [image_path]
* rotate [image_path] [angle]
* edge_detect [image_path]
* contrast [image_path] [delta]
* invert [image_path]
* network [xor/add]
* segmentation [image_path]
* network_load
* skew_detect [image_path]
* auto_rotate [image_path]

## Tests

To run tests, first you have to clone Unity at the root of the repository:
```
git clone https://github.com/ThrowTheSwitch/Unity.git
```

Then to run test:
```
make test
```

## Development
### Auto-format before commit
Add the following code to `.git/hooks/pre-commit` (create the file if not
already existing) and make it executable `chmod +x .git/hooks/pre-commit`.
```bash
#!/bin/sh

STAGED_FILES=$(git diff --name-only --cached)
make format
git add ${STAGED_FILES}
```

## Authors

[Flowtter](https://github.com/Flowtter)\
[aureleoules](https://github.com/aureleoules)\
[TrAyZeN](https://github.com/TrAyZeN)\
[Obeeron](https://github.com/Obeeron)

## License
[MIT](https://choosealicense.com/licenses/mit/)
