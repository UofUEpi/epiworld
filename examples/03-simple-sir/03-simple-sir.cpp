#define EPI_DEBUG
#include "../../include/epiworld/epiworld.hpp"

int main()
{

  // Creating a virus
  epiworld::Virus<> covid19("covid 19");
  covid19.set_prob_infecting(.8);
  covid19.set_status(1,2,2);
  
  // Creating a tool
  epiworld::Tool<> vax("vaccine");
  vax.set_susceptibility_reduction(.95);

  // Creating a model
  epiworld::Model<> model;

  model.add_status("Susceptible", epiworld::default_update_susceptible<>);
  model.add_status("Infected", epiworld::default_update_exposed<>);
  model.add_status("Removed");

  // Adding the tool and virus
  model.add_virus_n(covid19, 5);

  model.add_tool(vax, .5);

  // Generating a random pop
  model.agents_from_adjlist(
    epiworld::rgraph_smallworld(1000,5,.1,false,model)
  );

  // Initializing setting days and seed
  model.init(60, 123123);

  // Running the model
  model.run();

  model.print();

  return 0;
  
}