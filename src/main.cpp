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
    const Timer timer;  // progress reporting

    std::vector<const Point3d*> points = las.getPoints();
    timer.log("read las file");  // progress reporting

    KDTree* tree = KDTree::createTree(points);
    timer.log("built tree");  // progress reporting

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

    Timer const timerLoop;  // progress reporting
    size_t neighbors = 0;  // progress reporting
    size_t n = las.size();
    for (size_t i = 0; i < n; i++) {
        if (i % 10000 == 0) {
            // progress reporting (the whole if-block)
            double const avgNeighbors = (double)neighbors / i;
            neighbors = 0;
            fprintf(stderr, "\rCalculating normal for point: %lu / %lu. Time per point: %lf s. Average neighbors: %lf.", i, n, timerLoop.getDelta() / 1e6 / i, avgNeighbors);
        }

        const Point3d& p = las[i];
        std::vector<const Point3d*> const neighborhood = tree->getNeighbors(p, NORMAL_RADIUS);
        if (neighborhood.empty()) {
            // p is noise, remove p and don't add normal
            las.setIncluded(i, false);
            continue;
        }
        neighbors += neighborhood.size();  // progress reporting
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
            // p is noise, remove p and don't add normal
            las.setIncluded(i, false);
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

    timer.log("\rnormals calculated");  // progress reporting
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
            Color       pixelLowXLowY   = orthophoto.get_pixel(floor(pngX), floor(pngY));
            Color const pixelLowXHighY  = orthophoto.get_pixel(floor(pngX), ceil (pngY));
            Color       pixelHighXLowY  = orthophoto.get_pixel(ceil (pngX), floor(pngY));
            Color const pixelHighXHighY = orthophoto.get_pixel(ceil (pngX), ceil (pngY));
            misc::lerpPixelInplace(pixelLowXLowY,  pixelLowXHighY,  misc::frac(pngY));
            misc::lerpPixelInplace(pixelHighXLowY, pixelHighXHighY, misc::frac(pngY));
            misc::lerpPixelInplace(pixelLowXLowY, pixelHighXLowY, misc::frac(pngX));
            las.setColor(i, new Color(pixelLowXLowY));
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
