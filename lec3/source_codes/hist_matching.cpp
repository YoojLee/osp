#include "hist_func.h"

void hist_matching(Mat &input, Mat &matched, G *trans_func, float *CDF_ip, float *CDF_ref);

int main(){
    // prepare data
    Mat input = imread("img/input.jpg", CV_LOAD_IMAGE_COLOR);
    Mat reference = imread("img/reference.jpg", CV_LOAD_IMAGE_COLOR); 
    Mat input_gray, reference_gray;

    cvtColor(input, input_gray, CV_RGB2GRAY);
    cvtColor(reference, reference_gray, CV_RGB2GRAY);
    
    Mat matched = input_gray.clone();

    // PDF or transfer function txt files
    FILE *f_PDF = fopen("txt/PDF_before_matching.txt", "w+");
    FILE *f_matched_PDF_gray = fopen("txt/matched_PDF_gray.txt", "w+");
    FILE *f_trans_func_match = fopen("txt/trans_func_match.txt", "w+"); // inverse of G of T(r) (write 'r -> z')

    float *PDF_input = cal_PDF(input_gray);
    float *CDF_input = cal_CDF(input_gray);
    float *CDF_ref = cal_CDF(reference_gray);

    // trans_func_eq1, eq2 같은 경우엔 matching 내부에서 정의해도 큰 무리는 없을 것 같음.
    G trans_func_match[L] = { 0 }; // 최종적으로 구해야 하는 mapping (merge eq1 and eq2's inverse)

    hist_matching(input_gray, matched, trans_func_match, CDF_input, CDF_ref);
    float *matched_PDF_gray = cal_PDF(matched);

    for (int i = 0; i < L; i++){
        // write PDF
        fprintf(f_PDF, "%d\t%f\n", i, PDF_input[i]);
        fprintf(f_matched_PDF_gray, "%d\t%f\n", i, matched_PDF_gray[i]);

        // write transfer functions
        fprintf(f_trans_func_match, "%d\t%d\n", i, trans_func_match[i]);
    }

    // memory release
    free(PDF_input);
    free(CDF_input);
    free(CDF_ref);
    free(matched_PDF_gray);
    fclose(f_PDF);
    fclose(f_matched_PDF_gray);
    fclose(f_trans_func_match);

    // show image

    namedWindow("Original Image (Grayscale)", WINDOW_AUTOSIZE);
    imshow("Original Image (Grayscale)", input_gray);

    namedWindow("reference (Grayscale)", WINDOW_AUTOSIZE);
    imshow("reference (Grayscale)", reference_gray);
    imwrite("img/reference_gray.png", reference_gray);

    namedWindow("Matched", WINDOW_AUTOSIZE);
    imshow("Matched", matched);
    imwrite("res/matched_gray.png", matched);

    waitKey(0);

    return 0;
}

void hist_matching(Mat &input, Mat &matched, G *trans_func, float *CDF_ip, float *CDF_ref){
    
    // histogram equalization
    G trans_func_eq1[L] = { 0 }; 
    G trans_func_eq2[L] = { 0 };
    G trans_func_inv[L] = { (G)NULL };

    // compute transfer function
    for (int i = 0; i < L; i++){
        trans_func_eq1[i] = (G)((L-1)*CDF_ip[i]);
        trans_func_eq2[i] = (G)((L-1)*CDF_ref[i]);
    }

    // mapping from r to z
    /// 1. Compute G inverse.
    for (int i = 0; i < L; i++){
        trans_func_inv[trans_func_eq2[i]] = i;
    }

    /// 2. 1:1 매칭 아닌 부분 메꿔주기
    for (int i = 0; i < L; i++){
        if (trans_func_inv[i] == (G)NULL){
            if (i == 0 && trans_func_inv[i-1]== (G)NULL){ // 첫번째 intensity에서부터 값을 채워넣을 수 없는 경우 (즉, 255 값 또한 매핑이 안될 경우 identity mapping)
                trans_func_inv[i] = (G)(L-1);
            }
            trans_func_inv[i] = (G)(trans_func_inv[i-1]+1); // 값이 없는 경우엔 (이전값+1)으로 넣어줌.
        }
    }

    /// 3. merge (r to z)
    for (int i = 0; i < L; i++){
        trans_func[i] = trans_func_inv[trans_func_eq1[i]];
    }

    /// 4. apply the final transfer function
    for (int i = 0; i < input.rows; i++){
        for (int j = 0; j < input.cols; j++){
            matched.at<G>(i, j) = trans_func[input.at<G>(i, j)]; // G^(-1)(T(r))
        }
    }

    
}