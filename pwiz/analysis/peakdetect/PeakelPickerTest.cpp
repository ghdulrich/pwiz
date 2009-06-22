//
// PeakelPickerTest.cpp
//
//
// Original author: Darren Kessner <darren@proteowizard.org>
//
// Copyright 2009 Center for Applied Molecular Medicine
//   University of Southern California, Los Angeles, CA
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and 
// limitations under the License.
//


#include "PeakelPicker.hpp"
#include "pwiz/utility/misc/unit.hpp"
#include <iostream>
#include <cstring>
#include <iterator>


using namespace std;
using namespace pwiz::util;
using namespace pwiz::analysis;
using namespace pwiz::data::peakdata;
using boost::shared_ptr;


ostream* os_ = 0;


shared_ptr<PeakelField> createToyPeakelField()
{
    //
    //      0           1           2
    //      |.....:.....|.....:.....|
    // 10   x   x o 
    // 20   x   x o x
    // 30   x   x o x
    // 40   x   x o     <-- feature z==3, noise Peakel at mono+.5
    // 50
    // 60       x     x      
    // 70       x     x     x
    // 80       x     x     x
    // 90       x              <-- feature z==2
    //

    shared_ptr<PeakelField> toy(new PeakelField);

    PeakelPtr battery(new Peakel(Peak(0,10)));
    battery->peaks.push_back(Peak(0, 20));
    battery->peaks.push_back(Peak(0, 30));
    battery->peaks.push_back(Peak(0, 40));
    toy->insert(battery);

    battery.reset(new Peakel(Peak(1./3, 10)));
    battery->peaks.push_back(Peak(1./3, 20));
    battery->peaks.push_back(Peak(1./3, 30));
    battery->peaks.push_back(Peak(1./3, 40));
    toy->insert(battery);

    battery.reset(new Peakel(Peak(.5, 10)));
    battery->peaks.push_back(Peak(.5, 20));
    battery->peaks.push_back(Peak(.5, 30));
    battery->peaks.push_back(Peak(.5, 40));
    toy->insert(battery);
     
    battery.reset(new Peakel(Peak(2./3, 20)));
    battery->peaks.push_back(Peak(2./3, 30));
    toy->insert(battery);

    battery.reset(new Peakel(Peak(1./3, 60)));
    battery->peaks.push_back(Peak(1./3, 70));
    battery->peaks.push_back(Peak(1./3, 80));
    battery->peaks.push_back(Peak(1./3, 90));
    toy->insert(battery);

    battery.reset(new Peakel(Peak(1./3 + .5, 60)));
    battery->peaks.push_back(Peak(1./3 + .5, 70));
    battery->peaks.push_back(Peak(1./3 + .5, 80));
    toy->insert(battery);

    battery.reset(new Peakel(Peak(1./3 + 1, 70)));
    battery->peaks.push_back(Peak(1./3 + 1, 80));
    toy->insert(battery);

    if (os_)
    {
        *os_ << "createToyPeakelField(): " << toy->size() << endl;
        for (PeakelField::const_iterator it=toy->begin(); it!=toy->end(); ++it)
            *os_ << **it << endl;
    }

    return toy;
}


void testToy()
{
    if (os_) *os_ << "testToy()\n";

    PeakelPicker_Basic::Config config;
    // change config?

    PeakelPicker_Basic peterPiper(config);

    shared_ptr<PeakelField> peakels = createToyPeakelField();
    unit_assert(peakels->size() == 7);

    FeatureField peck;
    peterPiper.pick(*peakels, peck);

    if (os_)
    {
        *os_ << "after pick()\n";
        *os_ << "peakelField: " << peakels->size() << endl;
        for (PeakelField::const_iterator it=peakels->begin(); it!=peakels->end(); ++it)
            *os_ << **it << endl;

        *os_ << "featureField: " << peck.size() << endl;
        for (FeatureField::const_iterator it=peck.begin(); it!=peck.end(); ++it)
            *os_ << **it << endl;
    }

    unit_assert(peck.size() == 2);
    unit_assert(peakels->size() == 1);

    FeatureField::const_iterator it = peck.begin();
    unit_assert((*it)->mz == 0);
    unit_assert((*it)->charge == 3);
    unit_assert_equal((*it)->retentionTime, 25, 20);

    ++it;
    unit_assert((*it)->mz == 1./3);
    unit_assert((*it)->charge == 2);
    unit_assert_equal((*it)->retentionTime, 75, 20);
}


void test()
{
    testToy();
}


int main(int argc, char* argv[])
{
    try
    {
        if (argc>1 && !strcmp(argv[1],"-v")) os_ = &cout;
        test();
        return 0;
    }
    catch (exception& e)
    {
        cerr << e.what() << endl;
        return 1;
    }
}

