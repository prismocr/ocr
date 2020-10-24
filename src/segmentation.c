#include "segmentation.h"
#include "image.h"
#include "matrix.h"
#include "morphology.h"

/* LEO
 * Morphology based feature extraction method
 * Proposed by Wu, Hsieh and Chen:
 * https://www.cin.ufpe.br/~if751/projetos/artigos/Morphology-based%20text%20line%20extraction.pdf
 */
void feature_extraction(Matrix *image) {
    Matrix kernel, closed;

    kernel = structuring_element(3, 3);
    smooth(image, kernel);
    matrix_free(&kernel);

    kernel = structuring_element(1, 21);
    matrix_copy(*image, &closed);
    closing(&closed, kernel);
    opening(image, kernel);
    matrix_free(&kernel);

    difference(image, closed);
    matrix_free(&closed);

    kernel = structuring_element(5, 5);
    closing(image, kernel);
    matrix_free(&kernel);

    image_threshold(10.f, 255.f, image);
}
