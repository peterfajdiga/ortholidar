#include <iostream>
#include <vector>
#include <Eigen/Dense>
#include <Eigen/Eigenvalues>
#include <laswriter.hpp>
#include <lasreader.hpp>
#include "containers/Point3d.h"
#include "KDTree.h"
#include "Timer.h"
#include "containers/Vector3d.h"
#include "io/Las.h"


#define INPUT_FILENAME "../data/GKR_419_125.laz"
#define OUTPUT_FILENAME "../data/output.laz"
#define NORMAL_RADIUS 10.0
#define NOISE_THRESHOLD 200.0



void calculateNormals(const std::vector<Point3d>& points,
                      bool* const included,
                      std::vector<Vector3d>& normals) {
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

    std::vector<Point3d>::size_type const n = points.size();
    for (size_t i = 0; i < n; i++) {
        const Point3d& p = points[i];
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
            Vector3d normal(
                    eigenvectors(0,0),
                    eigenvectors(1,0),
                    eigenvectors(2,0)
            );
            normal.pointUp();
            normals.push_back(normal);
            included[i] = true;
        }
    }
    delete tree;
}


int main() {
    const Las las(INPUT_FILENAME);
    const std::vector<Point3d>& points = las.getPoints();

    std::vector<Point3d>::size_type const n = points.size();
    bool* included = new bool[n];
    for (size_t i = 0; i < n; i++) {
        included[i] = false;
    }

    std::vector<Vector3d> normals;
    calculateNormals(points, included, normals);

    las.savePoints(OUTPUT_FILENAME, included, normals);

    delete[] included;
    return 0;
}
