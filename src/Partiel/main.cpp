#include "pch.h"
#include "main.h"
#include "Affichage.h"
#include <SFML/Graphics.hpp>
#include <cmath>

// fonction qui calcule le cos d'un angle
// utilise la serie de taylor pour faire une approximation
void Cos(float angle, float& cosValue) {
    cosValue = 1.0f; // premier terme de la série
    float term = 1.0f; // def la valeur initiale de chaque terme
    for (int i = 1; i < 10; ++i) { // on prend 10 termes pour plus de précison
        term *= -angle * angle / (2 * i * (2 * i - 1)); // on calcule chaque terme de la série
        cosValue += term; // on ajoute chaque terme au résultat total
    }
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// fonction qui calcule le sinus d'un angle
// pareil que pour le cos utilisation de taylor
void Sin(float angle, float& sinValue) {
    sinValue = angle; // premier terme de la série
    float term = angle; // def la valeur initiale de chaque terme
    for (int i = 1; i < 10; ++i) { // encore une fois, 10 termes pour plus de précision
        term *= -angle * angle / ((2 * i) * (2 * i + 1)); // calcul du terme suivant
        sinValue += term; // on ajoute ce terme au resultat final
    }
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// dessine une ellipse avec un centre (xC, yC) et les demi-axes (a, b)
void Ellipse(float xC, float yC, float a, float b, Affichage& affichage) {
    std::vector<std::vector<float>> points; // liste pour stocker les points de l'ellipse
    int numPoints = 250; // on veut 250 points pour dessiner l'ellipse

    // on fait varier t de 0 à 250 
    for (int t = 0; t < numPoints; ++t) {
        // calcul de l'angle 
        float angle = t * 2 * 3.14159f / numPoints;

        // approximation des valeurs de cos(angle) et sin(angle) avec les fonctions définies
        float cosValue;
        float sinValue;
        Cos(angle, cosValue); // ici on appelle la fonction Cos pour obtenir cos(angle)
        Sin(angle, sinValue); // ici on appelle la fonction Sin pour obtenir sin(angle)

        // calcul des coordonnées (x, y)
        float x = xC + a * cosValue; // pour x, on applique l'approximation de cos
        float y = yC + b * sinValue; // pour y, on applique l'approximation de sin

        // on ajoute ce point à la liste des points
        points.push_back({ x, y });
    }

    // on affiche tous les points de l'ellipse
    affichage.add(points);
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Fonction qui réalise l'interpolation de Lagrange sur un ensemble de points
std::vector<std::vector<float>> Lagrange(std::vector<std::vector<float>> points, int NombreDePoint) {
    std::vector<std::vector<float>> result; // liste des points calculés par Lagrange
    int n = points.size();  // on récupère la taille de la liste de points

    // interpolation pour trouver le min et max de x
    float minX = points[0][0], maxX = points[0][0]; // on initialise min et max avec le premier point
    for (const auto& point : points) { // on regarde tous les points pour trouver le min et max
        if (point[0] < minX) minX = point[0]; // on cherche le min
        if (point[0] > maxX) maxX = point[0]; // et le max
    }

    // on calcule la distance entre chaque point
    float step = (maxX - minX) / (NombreDePoint - 1); // combien de distance entre chaque point qu'on va calculer

    // on calcule les points de la courbe
    for (int i = 0; i < NombreDePoint; ++i) {
        float x = minX + i * step; // on calcule x pour chaque point
        float y = 0; // on initialise y à 0, on va le calculer

        // polynôme de Lagrange pour trouver y pour chaque x
        for (int j = 0; j < n; ++j) {
            float term = points[j][1]; // on prend y du point j
            for (int k = 0; k < n; ++k) {
                if (k != j) { // on calcule le produit pour chaque terme de Lagrange
                    term *= (x - points[k][0]) / (points[j][0] - points[k][0]);
                }
            }
            y += term; // on ajoute le terme au résultat final de y
        }

        // on ajoute le point (x, y) dans la liste des résultats
        result.push_back({ x, y });
    }

    return result; // on retourne la liste de points calculés
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Fonction pour l'interpolation de Hermite
std::vector<std::vector<float>> Hermite(const std::vector<std::vector<float>>& points, const std::vector<float>& derivatives, int nbp){
    // Vérification des données
    if (points.size() != derivatives.size() || points.empty() || nbp < 2) {
        throw std::invalid_argument("Invalid input data");
    }

    std::vector<std::vector<float>> result; // Résultat des points interpolés
    int n = points.size();

    // Parcourir chaque segment entre deux points
    for (int i = 0; i < n - 1; ++i) {
        float x0 = points[i][0], y0 = points[i][1];      // Point de départ
        float x1 = points[i + 1][0], y1 = points[i + 1][1]; // Point d'arrivée
        float m0 = derivatives[i];                      // Dérivée en x0
        float m1 = derivatives[i + 1];                  // Dérivée en x1

        // Générer `nbp` points pour ce segment
        for (int j = 0; j < nbp; ++j) {
            float t = static_cast<float>(j) / (nbp - 1); // Paramètre normalisé [0, 1]

            // Polynômes de base de Hermite
            float h00 = (1 + 2 * t) * (1 - t) * (1 - t);
            float h10 = t * (1 - t) * (1 - t);
            float h01 = t * t * (3 - 2 * t);
            float h11 = t * t * (t - 1);

            // Calcul de la position interpolée
            float x = h00 * x0 + h10 * (x1 - x0) + h01 * x1 + h11 * (x1 - x0);
            float y = h00 * y0 + h10 * m0 * (x1 - x0) + h01 * y1 + h11 * m1 * (x1 - x0);

            result.push_back({ x, y });
        }
    }

    return result;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <iostream> // Pour l'entrée de la console

int main()
{
    // liste des points utilisés pour Lagrange et Hermite
    std::vector<std::vector<float>> liste = {};
    std::vector<std::vector<float>> points = {};
    std::vector<float> derivatives = {};

    // création de l'affichage
    Affichage affichage;

    // boucle qui tourne tant que la fenêtre est ouverte
    while (affichage.Win.isOpen())
    {
        // Demander à l'utilisateur quelle partie exécuter
        std::cout << "Choisissez la partie a lancer (1 pour Ellipse, 2 pour Lagrange, 3 pour Hermite, 4 pour quitter) : ";
        int choix;
        std::cin >> choix;
        std::cout << std::endl;

        // Effectuer des actions en fonction du choix
        if (choix == 1) {
            affichage.clear();

            // Partie 1: Dessiner l'ellipse
            std::cout << "Choisissez l'origin x :";
            int orx;
            std::cin >> orx;
            std::cout << std::endl;
            std::cout << "Choisissez l'origin y :";
            int ory;
            std::cin >> ory;
            std::cout << std::endl;
            std::cout << "size x :";
            int sx;
            std::cin >> sx;
            std::cout << std::endl;
            std::cout << "size y :";
            int sy;
            std::cin >> sy;
            std::cout << std::endl;
            std::cout << "Dessiner l'ellipse..." << std::endl;
            Ellipse(orx, ory, sx, sy, affichage);
        }
        if (choix == 2) {
            // Partie 2: Calculer et afficher la courbe Lagrange
            // Utile point : 1,5 // 4,2 // 3,-1 // 2,-2 // 0,-1 // -4,-3 // -4,0 // -2,2 // -1,3 // -2,3 // -5,1 // -6,1 // -6,2 // -5,2 // -2,4
            affichage.clear();
            
            points = {{4,2},{3,-1}, {2,-2}};
            affichage.addV(Lagrange(points, 250));

            points = { {2,-2},{0,-1} };
            derivatives = { -3,1 };
            affichage.add(Hermite(points, derivatives, 250));

            points = { {0,-1},{-2,-1}, {-4,-3} };
            affichage.addV(Lagrange(points, 250));

            points = { {-4,-3},{-2,2} };
            derivatives = { 15,-1 };
            affichage.add(Hermite(points, derivatives, 250));

            // Hermite entre -4,-3 et -2,2 (problemme lagrange impossible 2 x meme val = /0)
            
            affichage.add({ {-2,3},{-5,1} }); 
            
            
            
            points = { {-6,1},{-5,1} };
            derivatives =  {-1,1};
            affichage.addV(Hermite(points, derivatives, 250));

            // Hermite entre -2,2 et -2,3

            // Hermite entre -5,1 et -6,2 (problemme lagrange impossible 2 x meme val = /0)

            // Hermite entre -6,2 et -5,2

            // Hermite entre -5,2 et -2,4

            // Hermite entre -2,4 et 1,5

            

        }
        if (choix == 3) {
            affichage.clear();

            // Partie 3: Calculer et afficher la courbe Hermite


        }
        if (choix == 4) {
            // Partie 4: Quitter la boucle (fermer la fenêtre)
            std::cout << "Quitter..." << std::endl;
            affichage.Win.close();
        }
        if (choix != 1 && 2 && 3 && 4) {
            std::cout << "Choix invalide. Veuillez entrer un nombre entre 1 et 4." << std::endl;
        }

        // Mettre à jour l'affichage après chaque action
        affichage.Update();
    }

    return 0; // fin du programme
}

