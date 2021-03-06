#include "hist_func.h"

void linear_stretching(Mat &input, Mat &stretched, G *trans_func, G x1, G x2, G y1, G y2);

int main() {

	Mat input = imread("img/input.jpg", CV_LOAD_IMAGE_COLOR);
	Mat input_gray;

	cvtColor(input, input_gray, CV_RGB2GRAY);	// convert RGB to Grayscale

	Mat stretched = input_gray.clone();

	// PDF or transfer function txt files
	FILE *f_PDF;
	FILE *f_stretched_PDF;
	FILE *f_trans_func_stretch;

	f_PDF = fopen("txt/PDF.txt", "w+");
	f_stretched_PDF = fopen("txt/stretched_PDF.txt", "w+");
	f_trans_func_stretch = fopen("txt/trans_func_stretch.txt", "w+");

	G trans_func_stretch[L] = { 0 }; // 배열 형태로 mapping 정의 (index -> 배열 값)

	float *PDF = cal_PDF(input_gray);

	linear_stretching(input_gray, stretched, trans_func_stretch, 50, 110, 10, 110);	// histogram stretching (x1 ~ x2 -> y1 ~ y2)
	float *stretched_PDF = cal_PDF(stretched);										// stretched PDF

	for (int i = 0; i < L; i++) {
		// write PDF
		fprintf(f_PDF, "%d\t%f\n", i, PDF[i]);
		fprintf(f_stretched_PDF, "%d\t%f\n", i, stretched_PDF[i]);

		// write transfer functions
		fprintf(f_trans_func_stretch, "%d\t%d\n", i, trans_func_stretch[i]);
	}

	// memory release
	free(PDF);
	free(stretched_PDF);
	fclose(f_PDF);
	fclose(f_stretched_PDF);
	fclose(f_trans_func_stretch);
	
	////////////////////// Show each image ///////////////////////

	namedWindow("Grayscale", WINDOW_AUTOSIZE);
	imshow("Grayscale", input_gray);

	namedWindow("Stretched", WINDOW_AUTOSIZE);
	imshow("Stretched", stretched);
	imwrite("res/stretched.png", stretched);

	//////////////////////////////////////////////////////////////

	waitKey(0);

	return 0;
}

// histogram stretching (linear method)
void linear_stretching(Mat &input, Mat &stretched, G *trans_func, G x1, G x2, G y1, G y2) {

	float constant = (y2 - y1) / (float)(x2 - x1);

	// compute transfer function -> 범위에 따라 함수가 달라지므로, 범위에 따라 나눠서 다르게 대입해주기 (basically same tho)
	for (int i = 0; i < L; i++) {
		if (i >= 0 && i <= x1)
			trans_func[i] = (G)(y1 / x1 * i);
		else if (i > x1 && i <= x2)
			trans_func[i] = (G)(constant * (i - x1) + y1);
		else
			trans_func[i] = (G)((L - 1 - x2) / (L - 1 - y2) * (i - x2) + y2);
	}

	// perform the transfer function
	for (int i = 0; i < input.rows; i++)
		for (int j = 0; j < input.cols; j++)
			stretched.at<G>(i, j) = trans_func[input.at<G>(i, j)];
}