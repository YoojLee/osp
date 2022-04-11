# Pixel-wise Operation

## 프로젝트 구성
. <br>
├── exe <br>
├── img <br>
├── plot <br>
├── plot.ipynb <br>
├── res <br>
├── source_codes <br>
│   ├── hist_eq.cpp <br>
│   ├── hist_eq_RGB.cpp<br>
│   ├── hist_eq_YUV.cpp<br>
│   ├── hist_func.h<br>
│   ├── hist_matching_color.cpp<br>
│   ├── hist_matching.cpp<br>
│   ├── hist_stretching.cpp<br>
│   └── PDF_CDF.cpp<br>
└── txt


- exe: 실행파일 모아둔 폴더
- img: 입력 이미지 파일을 모아둔 폴더
- plot: pdf, cdf, function 등 plot을 모아둔 폴더
- res: pixel-wise operation 적용된 이미지 파일을 모아둔 폴더
- txt: pdf, function 등의 txt 파일을 모아둔 폴더
- source_codes: 소스파일(cpp, h)을 모아둔 폴더
    - hist_func.h: 이미지의 분포를 직접 계산하는 함수 정의
    - PDF_CDF.cpp: PDF, CDF를 계산 후 txt 파일로 작성
    - hist_stretching: grayscale 이미지를 입력으로 받아 linear stretching을 수행 후, pdf와 output을 파일 출력
    - hist_eq: grayscale 이미지를 입력으로 받아 histogram equalization(이하 HE)을 수행 후, pdf와 output을 파일 출력
    - hist_eq_RGB: RGB 이미지를 입력으로 받아 각 채널에 HE를 독립적으로 수행 후, pdf와 output을 파일 출력
    - hist_eq_YUV: RGB 이미지를 입력으로 받아 YUV 형식으로 변환 후 HE 수행 & pdf와 output을 파일 출력
    - hist_matching: grayscale 이미지와 reference 이미지(grayscale)를 입력으로 받아 histogram matching을 수행 후, pdf와 output을 파일 출력
    - hist_matching_color: RGB 이미지와 reference 이미지(RGB)를 입력으로 받아 YUV로 변환하여 histogram matching을 수행 후, pdf와 output을 파일 출력
- plot.ipynb: plotting을 수행하는 ipynb 파일

## 프로젝트 실행 환경
- OS: Ubuntu 18.04
- gcc version 7.5.0
- OpenCV 3.4.5
- IDE: VSCode
- plotting: Python 3.6.9, jupyter notebook
