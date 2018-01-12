//
// Created by peter on 11.1.2018.
//

#include "normals.h"

// free after use
Vector3d* calculateNormal(const KDTree* tree, const Point3d& p, size_t& n_neighbors) {
    std::vector<const Point3d*> const neighborhood = tree->getNeighbors(p, normals::NORMAL_RADIUS);
    if (neighborhood.empty()) {
        // p is noise
        return nullptr;
    }
    n_neighbors += neighborhood.size();  // progress reporting

    Eigen::Matrix3d covarianceMatrix;
    double& cov00 = covarianceMatrix(0,0);
    double& cov01 = covarianceMatrix(0,1);
    double& cov02 = covarianceMatrix(0,2);
    double& cov11 = covarianceMatrix(1,1);
    double& cov12 = covarianceMatrix(1,2);
    double& cov22 = covarianceMatrix(2,2);

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
    covarianceMatrix(1,0) = cov01;
    covarianceMatrix(2,0) = cov02;
    covarianceMatrix(2,1) = cov12;

    // the matrix is symmetric => we can use SelfAdjointEigenSolver
    Eigen::SelfAdjointEigenSolver<Eigen::Matrix3d> eigensolver(covarianceMatrix);
    assert (eigensolver.info() == Eigen::Success);
    // eigenvalues (and corresponding eigenvectors) are already sorted in increasing order
    double eigenvalue0 = eigensolver.eigenvalues()[0];
    double eigenvalue1 = eigensolver.eigenvalues()[1];
    assert (eigenvalue0 <= eigenvalue1 && eigenvalue1 <= eigensolver.eigenvalues()[2]);
    if (eigenvalue1 / eigenvalue0 < normals::NOISE_THRESHOLD) {
        // p is noise
        return nullptr;
    } else {
        const Eigen::Matrix3d& eigenvectors = eigensolver.eigenvectors();
        Vector3d* const normal = new Vector3d(
                eigenvectors(0,0),
                eigenvectors(1,0),
                eigenvectors(2,0)
        );
        normal->pointUp();
        return normal;
    }
}

// free after use
Vector3d* normals::calculateNormal(const KDTree* tree, const Point3d& p) {
    size_t ignore;
    return calculateNormal(tree, p, ignore);
}

void normals::calculateNormals(Las &las) {
    std::vector<const Point3d*> points = las.getPoints();

    const Timer timer_tree;  // progress reporting
    const KDTree* tree = KDTree::createTree(points);
    timer_tree.log("built tree");  // progress reporting

    size_t const n = las.size();
    size_t n_neighbors = 0;         // progress reporting
    char progressStringBuffer[20];  // progress reporting
    Timer const timer_normals;      // progress reporting
    fprintf(stderr, "Progress            | Time per point | Avg neighbors \n");
    for (size_t i = 0; i < n; i++) {
        if (i % 100000 == 0) {
            // progress reporting (the whole if-block)
            snprintf(progressStringBuffer, 20, "%lu / %lu", i, n);
            fprintf(stderr, "\r%19s | %14lf | %13lf", progressStringBuffer, timer_normals.getDelta() / 1e6 / i, (double)n_neighbors / i);
        }

        const Point3d& p = las[i];
        const Vector3d* const normal = calculateNormal(tree, p, n_neighbors);
        if (normal == nullptr) {
            // p is noise, remove p and don't add normal
            las.setIncluded(i, false);
        } else {
            las.setNormal(i, normal);
        }
    }
    delete tree;

    timer_normals.log("\nnormals calculated");  // progress reporting
}
