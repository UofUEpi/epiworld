// #include "../../include/epiworld/epiworld.hpp"
#include "../../include/epiworld/epiworld.hpp"
#include "../../include/epiworld/models/sir.hpp"
#include "../../include/epiworld/math/lfmcmc.hpp"

using namespace epiworld;

Model<> model;

size_t niter = 0;

std::vector< int > simfun(
    std::vector< epiworld_double > params,
    LFMCMC<std::vector<int>> * m
) {

    model("Immune recovery") = params[0u];
    model("Infectiousness")  = params[1u];

    model.reset();
    model.run();
           
    std::vector< int > res;
    model.get_db().get_today_total(nullptr, &res);

    return res;

}


void sumfun(
    std::vector< epiworld_double > & res,
    const std::vector< int > & dat,
    LFMCMC< std::vector<int> > * m
) {

    if (res.size() == 0u)
        res.resize(dat.size());

    
    for (size_t i = 0u; i < dat.size(); ++i)
        res[i] = static_cast< epiworld_double >(dat[i]);

    return;

}

// FUN<VEC( epiworld_double )(TData&, LFMCMC<TData>*)> summary_fun;

int main()
{

    set_up_sir(
        model,   // Model
        "covid", // Name of the virus
        .1,      // Initial prevalence
        .9,      // Infectiousness (par[1])
        0,       // Susceptibility reduction
        .5,      // Immune Recovery (par[0])
        1.0      // Post immunity
        );

    model.agents_smallworld(500);

    // Creating a new LFMCMC model
    LFMCMC<std::vector< int >> lfmcmc;

    lfmcmc.set_simulation_fun(simfun);
    lfmcmc.set_summary_fun(sumfun);
    // lfmcmc.set_proposal_fun(proposal_fun_unif<std::vector<int>>);
    lfmcmc.set_proposal_fun(make_proposal_norm_reflective<std::vector<int>>(.5, 0, 1));
    lfmcmc.set_kernel_fun(kernel_fun_gaussian<std::vector<int>>);

    // Simulating some data
    model.init(50, 122);
    model.set_backup();
    model.verbose_off();
    model.run();
    model.print();

    std::vector< int > obs_dat;
    model.get_db().get_today_total(nullptr, &obs_dat);

    lfmcmc.set_observed_data(obs_dat);

    std::vector< epiworld_double > par0 = {.5, .5};

    lfmcmc.run(par0, 1000, 1);
    
    lfmcmc.set_par_names({"Immune recovery", "Infectiousness"});
    lfmcmc.set_stats_names(model.get_status());

    lfmcmc.print();

  
}
