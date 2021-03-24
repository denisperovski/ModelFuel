#pragma once

#include <iostream>
#include <vector>

using namespace std;

typedef vector <tuple <string, double, double, double, double>> VectorFuel;
typedef vector <tuple <string, double, double, double>> VectorPpa;


class EcologyFuelClass
{
public:
    double Ch, Ck, Pp, Cp, L, v, G, MCo2, MH2O, MSo2, MCh, MSumm;

public:
    EcologyFuelClass (double Tyaga, double YdRashod, double Dlina, double Skorost)
    {
        Pp = Tyaga;
        Cp = YdRashod;
        L = Dlina;
        v = Skorost;
    }

    double FuelConsumption (double pl)//ищем расход
    {
        Ch = Cp * Pp / pl - 200;
        Ck = Ch / v;
        G = Ck * L;
        return G;
    }

    double MassOfEmissions()//ищем массу выбросов
    {
        MCo2 = 3.15 * G;
        MH2O = 1.23 * G;
        MSo2 = 0.0009 * G;
        MCh = 0.00021 * G;
        MSumm = MCo2 + MH2O + MSo2 + MCh;
        return MSumm;
    }

    void ResultSort(VectorFuel &fuel, unsigned int &nfuel)//сортируем по процентам и расходу
    {
        for (unsigned int i = 0; i < nfuel; i++)//метод пузырька
            for (unsigned int j = nfuel - 1; j >= i + 1; j--)
                if (get<2>(fuel[j]) < get<2>(fuel[j-1]))
                    fuel[j].swap(fuel[j-1]);

        while (nfuel > 5)//оставляем 5 топлив
        {
            fuel.erase(fuel.end() - 1);
            nfuel--;
        }

        for (unsigned int i =0; i < nfuel; i++)
            for (unsigned int j = nfuel - 1; j >= i + 1; j--)
                if (get<4>(fuel[j]) < get<4>(fuel[j-1]))
                    fuel[j].swap(fuel[j-1]);

        while (nfuel > 3)//оставляем 3 топлива
        {
            fuel.erase(fuel.end() - 1);
            nfuel--;
        }
    }
};
