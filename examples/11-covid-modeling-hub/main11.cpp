#define EPI_DEBUG
#include "../../include/epiworld/epiworld.hpp"

////////////////////////////////////////////////////////////////////////////////
/**
 * DETAILS OF THE MODEL
 * 
 * # ROWS
 * ====================
 * 
 * ## Optimistic waning of protection against infection
 * 
 * - Slow immune waning, median transition time to partially immune state =
 *  10 months
 * 
 * - In the partially immune state, there is a 40% reduciton in protection from
 *  baseline levels reported immediately after exposure (vaccination or
 *  infection.)
 * 
 * ## Pessimistic waning of protection against infection
 * 
 * - Fast immune waning, median transition time to partially immune state =
 *  4 months.
 * 
 * - In the partially immune state, there is a 60% reduction in protection from
 *  baseline levels reported immediately after exposure (vaccination or
 *  infection.)
 * 
 * # COLUMNS
 * ====================
 * 
 * ## No new variant
 * 
 * - Projections are initialized with the mix of strains circulating at the
 *  start of the projection period.
 * 
 * - New variant X emerges on May 1st, 2022. There is a continuous influx of 50
 *  weekly infections of variant X for the following 16 wks. Variant X has
 *  30% immune escape, and the same intrinsic transmissibility and severity as
 *  Omicron.
 */

// Designing variants ------------------------------------------------------
int main()
{   

    EPI_NEW_UPDATEFUN_LAMBDA(update_infected, int)
    {

        epiworld::VirusPtr<int> & v = p->get_virus(0);
        
        // Probability of infection
        m->array_double_tmp[0u] = p->get_recovery_enhancer(v);
        m->array_double_tmp[1u] = m->par("Infection Prob");
        
        int which = epiworld::roulette(2, m);

        // Becomes recovered
        if (which == 0)
            p->rm_virus(v);
        else // Becomes infected
            p->change_status(2);

        // Nothing happens
        return;
        
    };

    epiworld::Model<> model;
    model.add_status("Susceptible", epiworld::default_update_susceptible<>);
    model.add_status("Exposed", update_infected);
    model.add_status("Infected", epiworld::default_update_exposed<>);
    model.add_status("Recovered", epiworld::default_update_susceptible<>);
    model.add_status("Removed");

    model.add_param(14, "Emergence date");   // will be recorded as p0
    model.add_param(0, "Weekly count");      // will be recorded as p1
    model.add_param(0, "Total left");        // will be recorded as p2
    model.add_param(.4, "Omicron Immunity"); // will be recorded as p3
    model.add_param(.3, "Variant X scaped immunity"); // will be recorded as p4
    model.add_param(.2, "Infection Prob");

    // Variant X
    epiworld::Virus<> variant_x("X");
    variant_x.set_status(1,3,4);
    variant_x.set_post_immunity(.4);
    variant_x.set_prob_infecting(.9);
    variant_x.set_prob_recovery(.3);

    model.add_virus_n(variant_x, 0u); // Nobody has it at first

    // Delta
    epiworld::Virus<> variant_delta("Delta");
    variant_delta.set_status(1,3,4);
    variant_delta.set_post_immunity(.4);
    variant_delta.set_prob_infecting(.9);
    variant_delta.set_prob_recovery(.3);
    model.add_virus(variant_delta, .025);

    // Omicron
    epiworld::Virus<> variant_omicron("Omicron");
    variant_omicron.set_status(1,3,4);
    variant_omicron.set_post_immunity(.4);
    variant_omicron.set_prob_infecting(.9);
    variant_omicron.set_prob_recovery(.3);

    model.add_virus(variant_omicron, .05);
    

    // EPI_NEW_GLOBALFUN_LAMBDA(global_variant_x, int)
    // {
    //     // Identify an individual that will acquire the new variant
    //     int total_infected = m->get_db().get_today_total("Exposed");
    //     int who = static_cast<int>( total_infected * m->runif() );

    //     const auto & queue = m->get_queue();

    //     int cum_counts = 0;
    //     auto population = (*m->get_population());
    //     for (size_t i = 0u; i < m->size(); ++i)
    //         if (queue[i] > 0u)
    //         {
    //             // Is this agent infected
    //             auto & agent = population[i];
                
    //             if (agent.has_virus("Omicron"))
    //             {
    //                 if (++cum_counts == who)
    //                 {
    //                     // Replacing the virus
    //                     agent.get_virus(0) = variant_x;
    //                     break;
    //                 }
    //             }
                
    //         }

    // };

    // Setup
    model.population_smallworld();
    model.add_virus(epiworld::Virus<>(), 0.1);
    // model.add_tool(epiworld::Tool<>(), 1.0);

    // Initializing and running the model
    model.init(100, 226);
    model.run();

    // Printing the output
    model.print();

    return 0;

}