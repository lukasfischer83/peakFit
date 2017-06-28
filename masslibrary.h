#ifndef MASSLIBRARY_H
#define MASSLIBRARY_H

#include <QObject>
#include <QMap>


class element
{
public:
    explicit element(QString name, double mass, element* isotopeOf = NULL, double abundance = 1.0);
    double mass() const;
    QString name() const;

    element *isotopeOf() const;
    double abundance() const;

private:
    double m_mass;
    QString m_name;
    element* m_isotopeOf;
    double m_abundance;
};

typedef QMap<element*,int> composition_t;

class elementLibrary_t : public QMap<QString,element*>
{
public:
    explicit elementLibrary_t();

public:
    composition_t generateCompositionFromString(QString compositionString);

};

class molecule_t
{
public:
    explicit molecule_t(composition_t composition);
    explicit molecule_t();
    void addElement(element* e, int count);
    double mass() const;
    double isotopicAbundance() const;
    QString name() const;
    composition_t composition() const;
    int elementCountByName(QString name);



private:
    double calculateMass(composition_t composition);
    double calculateAbundance(composition_t composition);
    double nCr(int n, int k); // binomial coefficients n choose k;

    composition_t m_composition;
    double m_mass;
    double m_abundance;
};

class massLibrary : public QMap<double,molecule_t*>
{
public:
    explicit massLibrary();
    massLibrary::iterator append(molecule_t*m);
    elementLibrary_t elements() const;
    molecule_t* findClosestMolecule(double mass);
private:
    elementLibrary_t m_elements;
};


#endif // MASSLIBRARY_H
