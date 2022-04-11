#include "hist_func.h"

void hist_eq_Color(Mat &input, Mat &equalized, G(*trans_func)[3], float **CDF);

int main() {
	/* apply he for each channel in an independent way
	3개의 히스토그램, 3개의 pd */
	Mat input = imread("img/input.jpg", CV_LOAD_IMAGE_COLOR);
	Mat equalized_RGB = input.clone();

	// PDF or transfer function txt files
	FILE *f_equalized_PDF_RGB, *f_PDF_RGB;
	FILE *f_trans_func_eq_RGB;
	
	f_PDF_RGB = fopen("txt/PDF_RGB.txt", "w+");
	f_equalized_PDF_RGB = fopen("txt/equalized_PDF_RGB.txt", "w+");
	f_trans_func_eq_RGB = fopen("txt/trans_func_eq_RGB.txt", "w+");

	float **PDF_RGB = cal_PDF_RGB(input);	// PDF of Input image(RGB) : [L][3]
	float **CDF_RGB = cal_CDF_RGB(input);	// CDF of Input image(RGB) : [L][3]

	G trans_func_eq_RGB[L][3] = { 0 };		// transfer function

	// histogram equalization on RGB image
	hist_eq_Color(input, equalized_RGB, trans_func_eq_RGB, CDF_RGB);
	// equalized PDF (RGB)
	float **equalized_PDF_RGB = cal_PDF_RGB(equalized_RGB);
	
	for (int i = 0; i < L; i++) {
		for (int j = 0; j < 3; j++){
			// write PDF
			fprintf(f_PDF_RGB, "%d\t%d\t%f\n", i, j, PDF_RGB[i][j]);
			fprintf(f_equalized_PDF_RGB, "%d\t%d\t%f\n", i, j, equalized_PDF_RGB[i][j]);

			// write transfer functions
			fprintf(f_trans_func_eq_RGB, "%d\t%d\t%d\n", i, j, trans_func_eq_RGB[i][j]);
		}
		

		
	}

	// memory release
	free(PDF_RGB);
	free(CDF_RGB);
	fclose(f_PDF_RGB);
	fclose(f_equalized_PDF_RGB);
	fclose(f_trans_func_eq_RGB);

	////////////////////// Show each image ///////////////////////

	namedWindow("RGB", WINDOW_AUTOSIZE);
	imshow("RGB", input);

	namedWindow("Equalized_RGB", WINDOW_AUTOSIZE);
	imshow("Equalized_RGB", equalized_RGB);
	imwrite("res/equalized_rgb.png", equalized_RGB);

	//////////////////////////////////////////////////////////////

	waitKey(0);

	return 0;
}

// histogram equalization on 3 channel image
void hist_eq_Color(Mat &input, Mat &equalized, G(*trans_func)[3], float **CDF) {

	////////////////////////////////////////////////
	//											  //
	// How to access multi channel matrix element //
	//											  //
	// if matrix A is CV_8UC3 type,				  //
	// A(i, j, k) -> A.at<Vec3b>(i, j)[k]		  //
	//											  //
	////////////////////////////////////////////////

	// compute transfer function
	for (int i = 0; i < L; i++)
		for (int j = 0; j < 3; j++)
			trans_func[i][j] = (G)((L-1)*CDF[i][j]);
	
	// apply the transfer function
	for (int i = 0; i < input.rows; i++)
		for (int j = 0; j < input.cols; j++)
			for (int k = 0; k < 3; k++)
				equalized.at<C>(i, j)[k] = trans_func[input.at<C>(i, j)[k]][k];
		
}