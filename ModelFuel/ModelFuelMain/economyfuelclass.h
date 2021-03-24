#pragma once

#include <iostream>
#include <vector>

using namespace std;

typedef vector <tuple <string, double, double, double>> VectorFuel;//вектор кортежей

class EconomyFuelClass
{
public:
    double resultPrice;

    double ResultPrice(double Rashod, double Price)
    {
        resultPrice = Rashod * Price;
        return resultPrice;
    }

    void ResultSort(VectorFuel &fuel, unsigned int &nfuel)//метод пузырька
    {
        for (unsigned int i = 0; i < nfuel; i++)
            for (unsigned int j = nfuel - 1; j >= i + 1; j--)
                if (get<3>(fuel[j]) < get<3>(fuel[j-1]))
                    fuel[j].swap(fuel[j-1]);
    }
};
