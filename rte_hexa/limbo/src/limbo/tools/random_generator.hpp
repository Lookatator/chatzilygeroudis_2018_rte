//| This file is a part of the limbo framework.
//| Copyright 2014,  Universite Pierre et Marie Curie (UPMC) / Inria
//|
//| This software is a computer program whose purpose is to facilitate
//| experiments in evolutionary computation and evolutionary robotics.
//|
//| This software is governed by the CeCILL license under French law
//| and abiding by the rules of distribution of free software.  You
//| can use, modify and/ or redistribute the software under the terms
//| of the CeCILL license as circulated by CEA, CNRS and INRIA at the
//| following URL "http://www.cecill.info".
//|
//| As a counterpart to the access to the source code and rights to
//| copy, modify and redistribute granted by the license, users are
//| provided only with a limited warranty and the software's author,
//| the holder of the economic rights, and the successive licensors
//| have only limited liability.
//|
//| In this respect, the user's attention is drawn to the risks
//| associated with loading, using, modifying and/or developing or
//| reproducing the software by the user in light of its specific
//| status of free software, that may mean that it is complicated to
//| manipulate, and that also therefore means that it is reserved for
//| developers and experienced professionals having in-depth computer
//| knowledge. Users are therefore encouraged to load and test the
//| software's suitability as regards their requirements in conditions
//| enabling the security of their systems and/or data to be ensured
//| and, more generally, to use and operate it in the same conditions
//| as regards security.
//|
//| The fact that you are presently reading this means that you have
//| had knowledge of the CeCILL license and that you accept its terms.

#ifndef LIMBO_TOOLS_RANDOM_GENERATOR_HPP
#define LIMBO_TOOLS_RANDOM_GENERATOR_HPP

#include <cstdlib>
#include <cmath>
#include <ctime>
#include <list>
#include <stdlib.h>
#include <random>
#include <utility>
#include <mutex>
#include <limbo/tools/rand_utils.hpp>

namespace limbo {
    namespace tools {
        /// @ingroup tools
        /// a mt19937-based random generator (mutex-protected)
        ///
        /// usage :
        /// - RandomGenerator<double>(0.0, 1.0);
        /// - double r = rgen.rand();
        template <typename D>
        class RandomGenerator {
        public:
            using result_type = typename D::result_type;
            RandomGenerator(result_type min, result_type max) : _dist(min, max), _rgen(randutils::auto_seed_128{}.base()) {}
            result_type rand()
            {
                std::lock_guard<std::mutex> lock(_mutex);
                return _dist(_rgen);
            }

        private:
            D _dist;
            std::mt19937 _rgen;
            std::mutex _mutex;
        };

        /// @ingroup tools
        using rdist_double_t = std::uniform_real_distribution<double>;
        /// @ingroup tools
        using rdist_int_t = std::uniform_int_distribution<int>;

        /// @ingroup tools
        /// Double random number generator
        using rgen_double_t = RandomGenerator<rdist_double_t>;

        ///@ingroup tools
        ///integer random number generator
        using rgen_int_t = RandomGenerator<rdist_int_t>;

        /// @ingroup tools
        /// random vector in [0, 1]
        ///
        /// - this function is thread safe because the random number generator we use is thread-safe
        /// - we use a C++11 random number generator
        Eigen::VectorXd random_vector(int size)
        {
            static rgen_double_t rgen(0.0, 1.0);
            Eigen::VectorXd res(size);
            for (int i = 0; i < size; ++i)
                res[i] = rgen.rand();
            return res;
        }
    }
}

#endif
