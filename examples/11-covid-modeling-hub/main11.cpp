
#define EPI_DEBUG
#include "../../epiworld.hpp"

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

epiworld::Virus<int> * variant_x_ptr;
int weekly_infections_x = 50;
int weekly_infections_x_cum = 0;
int weekly_infections_start_day = 14; // Week 2 since the start of the model

// At first, we will pick an individual at random for starting the variant x.
// The virus will start from Omicron
EPI_NEW_GLOBALFUN(global_variant_x, int)
{
    // Identify an individual that will acquire the new variant
    int total_infected = m->get_db().get_today_total("exposed");
    int who = static_cast<int>( total_infected * m->runif() );

    const auto & queue = m->get_queue();

    int cum_counts = 0;
    auto & population = (*m->get_population());
    for (size_t i = 0u; i < m->size(); ++i)
        if (queue[i] > 0u)
        {
            // Is this person infected
            epiworld::Person<int> * person = &population[i];
            
            if (person->has_virus("Omicron"))
            {
                if (++cum_counts == who)
                {
                    // Replacing the virus
                    // person.get_virus(0) = *variant_x_ptr;
                    person->rm_virus(&person->get_virus(0));
                    person->add_virus(variant_x_ptr);
                    person->update_status(m->get_default_exposed());

                    break;
                }
            }
            
        }

}

// At the beginning of every week we reset the counter
EPI_NEW_GLOBALFUN(global_reset_count, int)
{
    if ((m->today() %  weekly_infections_start_day) == 0)
        weekly_infections_x_cum = 0;
}

// Infections will happen for sure if it is below the 50 mark
EPI_NEW_VIRUSFUN(transmission_of_x, int)
{
    if (weekly_infections_x_cum < weekly_infections_x)
    {
        return 1.0;
    } else
        return 0.0;
}

EPI_NEW_VIRUSFUN(recovery_of_x, int)
{
    if ((m->today() - v->get_date()) < 2)
        return 0.0;
    else
        return 0.3;
}

int main()
{
    // Designing variants ------------------------------------------------------
    // Delta
    epiworld::Virus<int> variant_delta("Delta");
    variant_delta.set_sequence(0);
    variant_delta.set_post_immunity(.4);
    variant_delta.set_prob_infecting(.95);
    variant_delta.set_prob_recovery(.2);

    // Omicron
    epiworld::Virus<int> variant_omicron("Omicron");
    variant_delta.set_sequence(1);
    variant_omicron.set_post_immunity(.4);
    variant_omicron.set_prob_infecting(.95);
    variant_omicron.set_prob_recovery(.2);

    // Variant X
    epiworld::Virus<int> variant_x("X");
    variant_x.set_sequence(2);
    variant_x.set_post_immunity(.4);
    variant_x.set_prob_recovery_fun(recovery_of_x);
    variant_x.set_prob_infecting_fun(transmission_of_x);
    variant_x_ptr = &variant_x;
    
    // Setup
    epiworld::Model<int> model;
    model.pop_from_random();

    model.add_virus(variant_delta, 0.1);
    model.add_virus(variant_omicron, 0.2);
    model.add_virus_n(variant_x, 0);

    model.add_global_action(global_reset_count, -1);
    model.add_global_action(global_variant_x, weekly_infections_start_day); // Appears at day 30

    // Initializing and running the model
    model.init(100, 226); 
    model.run();

    // Printing the output
    model.print();

    model.write_data("variants.csv", "variants_hist.csv", "total_hist.csv", "", "transition.csv");

    return 0;

}