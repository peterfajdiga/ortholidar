#include <iostream>
#include <vector>
#include <Eigen/Dense>
#include <png++/png.hpp>
#include "containers/Point3d.h"
#include "KDTree.h"
#include "Timer.h"
#include "containers/Vector3d.h"
#include "io/Las.h"
#include "containers/Color.h"


#define INPUT_LAS_FILENAME "../data/GKR_419_125.laz"
#define INPUT_PNG_FILENAME "../data/GKR_419_125.png"
#define OUTPUT_FILENAME "../data/output.laz"
#define NORMAL_RADIUS 10.0
#define NOISE_THRESHOLD 200.0



void calculateNormals(Las& las) {
    std::vector<const Point3d*> points = las.getPoints();
    KDTree* tree = KDTree::createTree(points);
    Eigen::Matrix3d covarianceMatrix;
    double& cov00 = covarianceMatrix(0,0);
    double& cov01 = covarianceMatrix(0,1);
    double& cov02 = covarianceMatrix(0,2);
    double& cov10 = covarianceMatrix(1,0);
    double& cov11 = covarianceMatrix(1,1);
    double& cov12 = covarianceMatrix(1,2);
    double& cov20 = covarianceMatrix(2,0);
    double& cov21 = covarianceMatrix(2,1);
    double& cov22 = covarianceMatrix(2,2);

    size_t n = las.size();
    for (size_t i = 0; i < n; i++) {
        const Point3d& p = las[i];
        std::vector<const Point3d*> const neighborhood = tree->getNeighbors(p, NORMAL_RADIUS);
        if (neighborhood.empty()) {
            // p is noise, don't add normal
            continue;
        }
        covarianceMatrix.setZero();  // zero initialize
        for (const Point3d* neighbor : neighborhood) {
            double const x = neighbor->x - p.x;
            double const y = neighbor->y - p.y;
            double const z = neighbor->z - p.z;
            cov00 += x * x;
            cov01 += x * y;
            cov02 += x * z;
            cov11 += y * y;
            cov12 += y * z;
            cov22 += z * z;
        }
        cov10 = cov01;
        cov20 = cov02;
        cov21 = cov12;

        // the matrix is symmetric => we can use SelfAdjointEigenSolver
        Eigen::SelfAdjointEigenSolver<Eigen::Matrix3d> eigensolver(covarianceMatrix);
        assert (eigensolver.info() == Eigen::Success);
        // eigenvalues (and corresponding eigenvectors) are already sorted in increasing order
        double eigenvalue0 = eigensolver.eigenvalues()[0];
        double eigenvalue1 = eigensolver.eigenvalues()[1];
        double eigenvalue2 = eigensolver.eigenvalues()[2];
        assert (eigenvalue0 <= eigenvalue1 && eigenvalue1 <= eigenvalue2);
        if (eigenvalue1 / eigenvalue0 > NOISE_THRESHOLD) {
            // p is noise, don't add normal
            continue;
        } else {
            const Eigen::Matrix3d& eigenvectors = eigensolver.eigenvectors();
            Vector3d* normal = new Vector3d(
                    eigenvectors(0,0),
                    eigenvectors(1,0),
                    eigenvectors(2,0)
            );
            normal->pointUp();
            las.setNormal(i, normal);
        }
    }
    delete tree;
}

void colorPoints(Las& las, const char* const pngFilename) {
    png::image<png::rgb_pixel_16> orthophoto(pngFilename);

    size_t const n = las.size();
    for (size_t i = 0; i < n; i++) {
        if (las.isIncluded(i)) {
            const Point3d& p = las[i];
            double const x01 =     (p.x - las.minX) / (las.maxX - las.minX);
            double const y01 = 1 - (p.y - las.minY) / (las.maxY - las.minY);
            double const pngX = x01 * (orthophoto.get_width()  - 1);
            double const pngY = y01 * (orthophoto.get_height() - 1);
            const png::rgb_pixel_16 pixel = orthophoto.get_pixel(pngX, pngY);
            las.setColor(i, new Color(pixel.red, pixel.green, pixel.blue));
        }
    }
}


int main() {
    Las las(INPUT_LAS_FILENAME);
//    calculateNormals(las);
    colorPoints(las, INPUT_PNG_FILENAME);
    las.save(OUTPUT_FILENAME);

    return 0;
}
