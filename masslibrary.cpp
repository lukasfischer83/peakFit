#include "masslibrary.h"

massLibrary::massLibrary() :
    QMap<double,molecule_t*>()
{
}

massLibrary::iterator massLibrary::append(molecule_t * m)
{
    // Check if all used elements are already in elementlist
    composition_t comp = m->composition();
    for (composition_t::iterator it = comp.begin();it != comp.end(); it++)
    {
        if (!m_elements.contains(it.key()->name()))
        {
            m_elements.insert(it.key()->name(),it.key());
        }
    }
    return this->insert(m->mass(),m);
}

elementLibrary_t massLibrary::elements() const
{
    return m_elements;
}

molecule_t *massLibrary::findClosestMolecule(double mass)
{
    double dist = 1e6;
    molecule_t* closestMolecule = this->first();
    //for (massLibrary::const_iterator it = this->begin();it!=this->end();it++)
    massLibrary::const_iterator it = this->lowerBound(mass);
    if (it != this->begin())
        it--;
    for (int i=0;i<5 && it != this->end();i++)
    {
        if (qAbs(it.key() - mass) < dist)
        {
            closestMolecule = it.value();
            dist = qAbs(it.key() - mass);
        }
        it++;
    }
    return closestMolecule;
}


element::element(QString name, double mass)
{
    m_name = name;
    m_mass = mass;
}

element::element(QString name, double mass, QMap<element *, double> isotopes)
{
    m_name = name;
    m_mass = mass;
    m_isotopes = isotopes;
}

double element::mass() const
{
    return m_mass;
}

QString element::name() const
{
    return m_name;
}


molecule_t::molecule_t(composition_t composition)
{
    m_composition = composition;
    m_mass = calculateMass(composition);
}

molecule_t::molecule_t()
{
    m_mass = 0;
}

void molecule_t::addElement(element *e, int count)
{
    m_composition.insert(e,count);
    m_mass = calculateMass(m_composition);
}

double molecule_t::mass() const
{
    return m_mass;
}

QString molecule_t::name() const
{
    QMap<QString,composition_t::const_iterator> sortedElements;

    for (composition_t::const_iterator it = m_composition.begin(); it != m_composition.end(); it++)
    {
        sortedElements.insert(it.key()->name(),it);
    }

    QString n = "";
    QString ion = " ";
    for (QMap<QString,composition_t::const_iterator>::const_iterator it = sortedElements.begin(); it != sortedElements.end(); it++)
    {

        if (it.key().contains('+') || it.key().contains('-'))
        {
            ion.append(it.key()).append(" ");
        } else {
        if (it.value().value() == 0)
        {
        }
        else if (it.value().value()==1)
        {
            n.append(it.value().key()->name());
        }
        else
        {
            n.append(it.value().key()->name()).append(QString::number(it.value().value()));
        }
        }
    }

    return n.append(ion);
}

composition_t molecule_t::composition() const
{
    return m_composition;
}

int molecule_t::elementCountByName(QString name)
{
    for(composition_t::const_iterator it = m_composition.begin(); it != m_composition.end(); it++)
    {
        if(it.key()->name() == name)
            return it.value();
    }
    return 0;
}

double molecule_t::calculateMass(composition_t composition)
{
    double mass=0;

    for (composition_t::iterator it = composition.begin(); it != composition.end(); it++)
    {
        mass += it.key()->mass() * it.value();
    }
    return mass;
}


elementLibrary_t::elementLibrary_t() : QMap<QString,element*>()
{

}

composition_t elementLibrary_t::generateCompositionFromString(QString compositionString)
{
       // implement Chemical Formula Parser !!!!!
    return QMap<element*,int>();
}


