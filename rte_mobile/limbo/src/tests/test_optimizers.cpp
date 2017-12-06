//| Copyright Inria May 2015
//| This project has received funding from the European Research Council (ERC) under
//| the European Union's Horizon 2020 research and innovation programme (grant
//| agreement No 637972) - see http://www.resibots.eu
//|
//| Contributor(s):
//|   - Jean-Baptiste Mouret (jean-baptiste.mouret@inria.fr)
//|   - Antoine Cully (antoinecully@gmail.com)
//|   - Kontantinos Chatzilygeroudis (konstantinos.chatzilygeroudis@inria.fr)
//|   - Federico Allocati (fede.allocati@gmail.com)
//|   - Vaios Papaspyros (b.papaspyros@gmail.com)
//|   - Roberto Rama (bertoski@gmail.com)
//|
//| This software is a computer library whose purpose is to optimize continuous,
//| black-box functions. It mainly implements Gaussian processes and Bayesian
//| optimization.
//| Main repository: http://github.com/resibots/limbo
//| Documentation: http://www.resibots.eu/limbo
//|
//| This software is governed by the CeCILL-C license under French law and
//| abiding by the rules of distribution of free software.  You can  use,
//| modify and/ or redistribute the software under the terms of the CeCILL-C
//| license as circulated by CEA, CNRS and INRIA at the following URL
//| "http://www.cecill.info".
//|
//| As a counterpart to the access to the source code and  rights to copy,
//| modify and redistribute granted by the license, users are provided only
//| with a limited warranty  and the software's author,  the holder of the
//| economic rights,  and the successive licensors  have only  limited
//| liability.
//|
//| In this respect, the user's attention is drawn to the risks associated
//| with loading,  using,  modifying and/or developing or reproducing the
//| software by the user in light of its specific status of free software,
//| that may mean  that it is complicated to manipulate,  and  that  also
//| therefore means  that it is reserved for developers  and  experienced
//| professionals having in-depth computer knowledge. Users are therefore
//| encouraged to load and test the software's suitability as regards their
//| requirements in conditions enabling the security of their systems and/or
//| data to be ensured and,  more generally, to use and operate it in the
//| same conditions as regards security.
//|
//| The fact that you are presently reading this means that you have had
//| knowledge of the CeCILL-C license and that you accept its terms.
//|
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE test_optimizers

#include <boost/test/unit_test.hpp>

#include <limbo/opt/chained.hpp>
#include <limbo/opt/cmaes.hpp>
#include <limbo/opt/grid_search.hpp>
#include <limbo/opt/random_point.hpp>
#include <limbo/tools/macros.hpp>

using namespace limbo;

struct Params {
    struct opt_gridsearch {
        BO_PARAM(int, bins, 20);
    };
};

// test with a standard function
int monodim_calls = 0;
opt::eval_t acqui_mono(const Eigen::VectorXd& v, bool eval_grad)
{
    assert(!eval_grad);
    monodim_calls++;
    return opt::no_grad(3 * v(0) + 5);
}

// test with a functor
int bidim_calls = 0;
struct FakeAcquiBi {
    opt::eval_t operator()(const Eigen::VectorXd& v, bool eval_grad) const
    {
        assert(!eval_grad);
        bidim_calls++;
        return opt::no_grad(3 * v(0) + 5 - 2 * v(1) - 5 * v(1) + 2);
    }
};

BOOST_AUTO_TEST_CASE(test_random_mono_dim)
{
    using namespace limbo;

    opt::RandomPoint<Params> optimizer;

    monodim_calls = 0;
    for (int i = 0; i < 1000; i++) {
        Eigen::VectorXd best_point = optimizer(acqui_mono, Eigen::VectorXd::Constant(1, 0.5), true);
        BOOST_CHECK_EQUAL(best_point.size(), 1);
        BOOST_CHECK(best_point(0) > 0 || std::abs(best_point(0)) < 1e-7);
        BOOST_CHECK(best_point(0) < 1 || std::abs(best_point(0) - 1) < 1e-7);
    }
}

BOOST_AUTO_TEST_CASE(test_random_bi_dim)
{
    using namespace limbo;

    opt::RandomPoint<Params> optimizer;

    bidim_calls = 0;
    for (int i = 0; i < 1000; i++) {
        Eigen::VectorXd best_point = optimizer(FakeAcquiBi(), Eigen::VectorXd::Constant(2, 0.5), true);
        BOOST_CHECK_EQUAL(best_point.size(), 2);
        BOOST_CHECK(best_point(0) > 0 || std::abs(best_point(0)) < 1e-7);
        BOOST_CHECK(best_point(0) < 1 || std::abs(best_point(0) - 1) < 1e-7);
        BOOST_CHECK(best_point(1) > 0 || std::abs(best_point(1)) < 1e-7);
        BOOST_CHECK(best_point(1) < 1 || std::abs(best_point(1) - 1) < 1e-7);
    }
}

BOOST_AUTO_TEST_CASE(test_grid_search_mono_dim)
{
    using namespace limbo;

    opt::GridSearch<Params> optimizer;

    monodim_calls = 0;
    Eigen::VectorXd best_point = optimizer(acqui_mono, Eigen::VectorXd::Constant(1, 0.5), true);

    BOOST_CHECK_EQUAL(best_point.size(), 1);
    BOOST_CHECK_CLOSE(best_point(0), 1, 0.0001);
    BOOST_CHECK_EQUAL(monodim_calls, Params::opt_gridsearch::bins() + 1);
}

BOOST_AUTO_TEST_CASE(test_grid_search_bi_dim)
{
    using namespace limbo;

    opt::GridSearch<Params> optimizer;

    bidim_calls = 0;
    Eigen::VectorXd best_point = optimizer(FakeAcquiBi(), Eigen::VectorXd::Constant(2, 0.5), true);

    BOOST_CHECK_EQUAL(best_point.size(), 2);
    BOOST_CHECK_CLOSE(best_point(0), 1, 0.0001);
    BOOST_CHECK_SMALL(best_point(1), 0.000001);
    // TO-DO: Maybe alter a little grid search so not to call more times the utility function
    BOOST_CHECK_EQUAL(bidim_calls, (Params::opt_gridsearch::bins() + 1) * (Params::opt_gridsearch::bins() + 1) + 21);
}

BOOST_AUTO_TEST_CASE(test_chained)
{
    using namespace limbo;

    using opt_1_t = opt::GridSearch<Params>;
    using opt_2_t = opt::RandomPoint<Params>;
    using opt_3_t = opt::GridSearch<Params>;
    using opt_4_t = opt::GridSearch<Params>;
    opt::Chained<Params, opt_1_t, opt_2_t, opt_3_t, opt_4_t> optimizer;

    monodim_calls = 0;
    Eigen::VectorXd best_point = optimizer(acqui_mono, Eigen::VectorXd::Constant(1, 0.5), true);

    BOOST_CHECK_EQUAL(best_point.size(), 1);
    BOOST_CHECK(best_point(0) > 0 || std::abs(best_point(0)) < 1e-7);
    BOOST_CHECK(best_point(0) < 1 || std::abs(best_point(0) - 1) < 1e-7);
    BOOST_CHECK_EQUAL(monodim_calls, (Params::opt_gridsearch::bins() + 1) * 3);
}
