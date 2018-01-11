#include <png++/png.hpp>
#include "containers/Point3d.h"
#include "Timer.h"
#include "operations/misc.h"
#include "operations/normals.h"



void printNeighborhoodNormals(const Las& las, size_t const index, const char* const pointsFilename, const char* normalsFilename) {
    std::vector<const Point3d*> points = las.getPoints();
    const KDTree* tree = KDTree::createTree(points);
    const std::vector<const Point3d*> neighborhood = tree->getNeighbors(las[index], normals::NORMAL_RADIUS);

    FILE* pointsFile = fopen(pointsFilename, "w");
    if (pointsFile == nullptr) {
        fprintf(stderr, "Error opening file %s\n", pointsFilename);
        exit(1);
    }
    FILE* normalsFile = fopen(normalsFilename, "w");
    if (normalsFile == nullptr) {
        fprintf(stderr, "Error opening file %s\n", normalsFilename);
        exit(1);
    }

    for (const Point3d* const p : neighborhood) {
        const Vector3d* const n = normals::calculateNormal(tree, *p);
        if (n != nullptr) {
            fprintf(pointsFile , "%lf, %lf, %lf\n", p->x, p->y, p->z);
            fprintf(normalsFile, "%lf, %lf, %lf\n", n->x, n->y, n->z);
            delete n;
        }
    }

    fclose(pointsFile);
    fclose(normalsFile);
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
    normals::calculateNormals(las);
    colorPoints(las, argv[2]);
    las.save(argv[3]);
    //printNeighborhoodNormals(las, 2100531, "../data/matlab/points.csv", "../data/matlab/normals.csv");

    return 0;
}
