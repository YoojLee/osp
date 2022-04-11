#include "hist_func.h"

void hist_matching(Mat &input, Mat &matched, G *trans_func, float *CDF_ip, float *CDF_ref);

int main(){
    // prepare data
    Mat input = imread("img/input.jpg", CV_LOAD_IMAGE_COLOR);
    Mat reference = imread("img/reference.jpg", CV_LOAD_IMAGE_COLOR); 
    Mat matched, eq_ref;

    cvtColor(input, matched, CV_RGB2YUV);
    cvtColor(reference, eq_ref, CV_RGB2YUV);

    // split each channel (Y,U,V)
    Mat channels_ip[3], channels_ref[3];
    split(matched, channels_ip);
    split(eq_ref, channels_ref);
    Mat Y_ip = channels_ip[0];
    Mat Y_ref = channels_ref[0];

    // PDF or transfer function txt files
    FILE *f_PDF = fopen("txt/PDF_before_matching_color.txt", "w+");
    FILE *f_matched_PDF = fopen("txt/matched_PDF_color.txt", "w+");
    FILE *f_trans_func_match = fopen("txt/trans_func_match_color.txt", "w+"); // inverse of G of T(r) (write 'r -> z')

    float **PDF_input = cal_PDF_RGB(input);
    float *CDF_ip = cal_CDF(Y_ip);
    float *CDF_ref = cal_CDF(Y_ref);

    // trans_func_eq1, eq2 같은 경우엔 matching 내부에서 정의해도 큰 무리는 없을 것 같음.
    G trans_func_match[L] = { 0 }; // 최종적으로 구해야 하는 mapping (merge eq1 and eq2's inverse)

    // histogram matching
    hist_matching(Y_ip, channels_ip[0], trans_func_match, CDF_ip, CDF_ref);

    // merge Y, U, V channels
    merge(channels_ip, 3, matched);

    // YUV -> RGB
    cvtColor(matched, matched, CV_YUV2RGB);

    float **matched_PDF = cal_PDF_RGB(matched);

    for (int i = 0; i < L; i++){
        // write PDF
        for (int j = 0; j < 3; j++){
            fprintf(f_PDF, "%d\t%d\t%f\n", i, j, PDF_input[i][j]);
            fprintf(f_matched_PDF, "%d\t%d\t%f\n", i, j, matched_PDF[i][j]);
        }

        // write transfer functions
        fprintf(f_trans_func_match, "%d\t%d\n", i, trans_func_match[i]);
    }

    // memory release
    free(PDF_input);
    free(CDF_ip);
    free(CDF_ref);
    free(matched_PDF);
    fclose(f_PDF);
    fclose(f_matched_PDF);
    fclose(f_trans_func_match);

    // show image

    namedWindow("Original Image (Grayscale)", WINDOW_AUTOSIZE);
    imshow("Original Image (Grayscale)", input);

    namedWindow("reference (Grayscale)", WINDOW_AUTOSIZE);
    imshow("reference (Grayscale)", reference);

    namedWindow("Matched", WINDOW_AUTOSIZE);
    imshow("Matched", matched);
    imwrite("res/matched_color.png", matched);

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