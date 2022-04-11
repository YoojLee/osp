#include "hist_func.h"

void hist_eq(Mat &input, Mat &equalized, G *trans_func, float *CDF);

int main() {
	/* 대체적으로 main은 변수 선언하고 이미지 불러서 읽어오고 pdf&cdf 계산 후에 
	equalization 수행하고 다시 file로 write & show results해주는 형식*/

	Mat input = imread("img/input.jpg", CV_LOAD_IMAGE_COLOR); // 이미지 읽기 (in rgb)
	Mat input_gray; // input_gray라는 Mat 타입 변수 선언

	// 바로 assign을 하는 형식이 아니라 input과 input_gray를 인자로 받아서 안에서 그냥 input_gray가 grayscale의 input을 가리키도록 함
	cvtColor(input, input_gray, CV_RGB2GRAY);	// convert RGB to Grayscale

	Mat equalized = input_gray.clone(); // copy해서 equalized에 넣어줌.

	// PDF or transfer function txt files
	FILE *f_PDF=fopen("txt/PDF.txt", "w+");
	FILE *f_equalized_PDF_gray=fopen("txt/equalized_PDF_gray.txt", "w+");
	FILE *f_trans_func_eq=fopen("txt/trans_func_eq.txt", "w+");
	

	float *PDF = cal_PDF(input_gray);	// PDF of Input image(Grayscale) : [L]
	float *CDF = cal_CDF(input_gray);	// CDF of Input image(Grayscale) : [L]

	G trans_func_eq[L] = { 0 };			// transfer function

	hist_eq(input_gray, equalized, trans_func_eq, CDF);								// histogram equalization on grayscale image
	float *equalized_PDF_gray = cal_PDF(equalized);									// equalized PDF (grayscale)

	for (int i = 0; i < L; i++) {
		// write PDF
		fprintf(f_PDF, "%d\t%f\n", i, PDF[i]);
		fprintf(f_equalized_PDF_gray, "%d\t%f\n", i, equalized_PDF_gray[i]);

		// write transfer functions
		fprintf(f_trans_func_eq, "%d\t%d\n", i, trans_func_eq[i]);
	}

	// memory release
	free(PDF);
	free(CDF);
	fclose(f_PDF);
	fclose(f_equalized_PDF_gray);
	fclose(f_trans_func_eq);

	////////////////////// Show each image ///////////////////////

	namedWindow("Grayscale", WINDOW_AUTOSIZE);
	imshow("Grayscale", input_gray);

	namedWindow("Equalized", WINDOW_AUTOSIZE);
	imshow("Equalized", equalized);
	imwrite("res/equalized.png", equalized);

	//////////////////////////////////////////////////////////////

	waitKey(0);

	return 0;
}

// histogram equalization
void hist_eq(Mat &input, Mat &equalized, G *trans_func, float *CDF) {

	// compute transfer function
	for (int i = 0; i < L; i++)
		trans_func[i] = (G)((L - 1) * CDF[i]);

	// perform the transfer function
	for (int i = 0; i < input.rows; i++)
		for (int j = 0; j < input.cols; j++)
			equalized.at<G>(i, j) = trans_func[input.at<G>(i, j)]; // T(r) 대입. r은 original intensity
}