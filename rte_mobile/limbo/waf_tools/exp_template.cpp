// please see the explanation in the documentation
// http://www.resibots.eu/limbo

#include <iostream>

// you can also include <limbo/limbo.hpp> but it will slow down the compilation
#include <limbo/bayes_opt/boptimizer.hpp>

using namespace limbo;

struct Params {
    struct bayes_opt_boptimizer : public defaults::bayes_opt_boptimizer {
        @BAYES_OPT_BOPTIMIZER_NOISE
    };

// depending on which internal optimizer we use, we need to import different parameters
#ifdef USE_LIBCMAES
    struct opt_cmaes : public defaults::opt_cmaes {
    };
#elif defined(USE_NLOPT)
    struct opt_nloptnograd : public defaults::opt_nloptnograd {
    };
#else
    struct opt_gridsearch : public defaults::opt_gridsearch {
    };
#endif

    struct bayes_opt_bobase : public defaults::bayes_opt_bobase {
        @BAYES_OPT_BOBASE_STATS_DISABLED
    };

    struct kernel_exp : public defaults::kernel_exp {
    };

    struct init_randomsampling : public defaults::init_randomsampling {
        @INIT_RANDOMSAMPLING_SAMPLES
    };

    struct stop_maxiterations : public defaults::stop_maxiterations {
        @STOP_MAXITERATIONS_ITERATIONS
    };

    // we use the default parameters for acqui_ucb
    struct acqui_ucb : public defaults::acqui_ucb {
    };
};

struct Eval {
    // number of input dimension (x.size())
    static constexpr size_t dim_in = @DIM_IN;
    // number of dimenions of the result (res.size())
    static constexpr size_t dim_out = @DIM_OUT;

    // the function to be optimized
    Eigen::VectorXd operator()(const Eigen::VectorXd& x) const
    {
        @CODE_RES_INIT

            // YOUR CODE HERE

            @CODE_RES_RETURN
    }
};

int main()
{
    // we use the default acquisition function / model / stat / etc.
    bayes_opt::BOptimizer<Params> boptimizer;
    // run the evaluation
    boptimizer.optimize(Eval());
    // the best sample found
    std::cout << "Best sample: " << @CODE_BEST_SAMPLE << " - Best observation: " << @CODE_BEST_OBS << std::endl;
    return 0;
}
