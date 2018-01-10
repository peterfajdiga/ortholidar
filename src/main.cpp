#include <iostream>
#include <vector>
#include <Eigen/Dense>
#include <png++/png.hpp>
#include "containers/Point3d.h"
#include "KDTree.h"
#include "Timer.h"
#include "misc.h"


#define NORMAL_RADIUS 10.0
#define NOISE_THRESHOLD 200.0



void calculateNormals(Las& las) {
    const Timer timer;

    std::vector<const Point3d*> points = las.getPoints();
    timer.log("read las file");

    KDTree* tree = KDTree::createTree(points);
    timer.log("built tree");

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

    timer.log("normals calculated");
}

void colorPoints(Las& las, const char* const pngFilename) {
    png::image<png::rgb_pixel_16> orthophoto(pngFilename);

    size_t const n = las.size();
    for (size_t i = 0; i < n; i++) {
        if (las.isIncluded(i)) {
            const Point3d& p = las[i];
            double const x01 =     (p.x - las.getMinX()) / (las.getMaxX() - las.getMinX());
            double const y01 = 1 - (p.y - las.getMinY()) / (las.getMaxY() - las.getMinY());
            double const pngX = x01 * (orthophoto.get_width()  - 1);  // x coordinate in png space
            double const pngY = y01 * (orthophoto.get_height() - 1);  // y coordinate in png space
            png::rgb_pixel_16 const pixelLowXLowY   = orthophoto.get_pixel(floor(pngX), floor(pngY));
            png::rgb_pixel_16 const pixelLowXHighY  = orthophoto.get_pixel(floor(pngX), ceil (pngY));
            png::rgb_pixel_16 const pixelHighXLowY  = orthophoto.get_pixel(ceil (pngX), floor(pngY));
            png::rgb_pixel_16 const pixelHighXHighY = orthophoto.get_pixel(ceil (pngX), ceil (pngY));
            png::rgb_pixel_16 const pixelLowX  = misc::lerpPixel(pixelLowXLowY,  pixelLowXHighY,  misc::frac(y01));
            png::rgb_pixel_16 const pixelHighX = misc::lerpPixel(pixelHighXLowY, pixelHighXHighY, misc::frac(y01));
            png::rgb_pixel_16 const pixel = misc::lerpPixel(pixelLowX, pixelHighX, misc::frac(x01));
            las.setColor(i, new Color(pixel.red, pixel.green, pixel.blue));
        }
    }
}


int main(int const argc, const char* const argv[]) {
    if (argc != 4) {
        std::cerr << "Usage: ortholidar LASFILE ORTHOPHOTO OUTPUTFILE\n";
        exit(1);  // TODO: define error code
    }

    Las las(argv[1]);
    calculateNormals(las);
    colorPoints(las, argv[2]);
    las.save(argv[3]);

    return 0;
}
