#ifndef EPIWORLD_RANDGRA
#define EPIWORLD_RANDGRA

template<typename TSeq>
class Model;

template<typename TSeq>
class Agent;

class AdjList;


template<typename TSeq, typename TDat>
inline void rewire_degseq(
    TDat * agents,
    Model<TSeq> * model,
    epiworld_double proportion
    );

template<typename TSeq = int>
inline void rewire_degseq(
    std::vector< Agent<TSeq> > * agents,
    Model<TSeq> * model,
    epiworld_double proportion
    )
{

    // Identifying individuals with degree > 0
    std::vector< unsigned int > non_isolates;
    std::vector< epiworld_double > weights;
    epiworld_double nedges = 0.0;
    // std::vector< Agent<TSeq> > * agents = model->get_agents();
    for (unsigned int i = 0u; i < agents->size(); ++i)
    {
        if (agents->operator[](i).get_neighbors().size() > 0u)
        {
            non_isolates.push_back(i);
            epiworld_double wtemp = static_cast<epiworld_double>(agents->operator[](i).get_neighbors().size());
            weights.push_back(wtemp);
            nedges += wtemp;
        }
    }

    if (non_isolates.size() == 0u)
        throw std::logic_error("The graph is completely disconnected.");

    // Cumulative probs
    weights[0u] /= nedges;
    for (unsigned int i = 1u; i < non_isolates.size(); ++i)
    {
         weights[i] /= nedges;
         weights[i] += weights[i - 1u];
    }

    // Only swap if needed
    unsigned int N = non_isolates.size();
    epiworld_double prob;
    int nrewires = floor(proportion * nedges);
    while (nrewires-- > 0)
    {

        // Picking egos
        prob = model->runif();
        int id0 = N - 1;
        for (unsigned int i = 0u; i < N; ++i)
            if (prob <= weights[i])
            {
                id0 = i;
                break;
            }

        prob = model->runif();
        int id1 = N - 1;
        for (unsigned int i = 0u; i < N; ++i)
            if (prob <= weights[i])
            {
                id1 = i;
                break;
            }

        // Correcting for under or overflow.
        if (id1 == id0)
            id1++;

        if (id1 >= static_cast<int>(N))
            id1 = 0;

        Agent<TSeq> & p0 = agents->operator[](non_isolates[id0]);
        Agent<TSeq> & p1 = agents->operator[](non_isolates[id1]);

        // Picking alters (relative location in their lists)
        // In this case, these are uniformly distributed within the list
        int id01 = std::floor(p0.get_neighbors().size() * model->runif());
        int id11 = std::floor(p1.get_neighbors().size() * model->runif());

        // When rewiring, we need to flip the individuals from the other
        // end as well, since we are dealing withi an undirected graph
        
        // Finding what neighbour is id0
        if (!model->is_directed())
        {
            // Picking 0's alter
            unsigned int n0,n1;
            Agent<TSeq> & p01 = agents->operator[](p0.get_neighbors()[id01]->get_id());
            for (n0 = 0; n0 < p01.get_neighbors().size(); ++n0)
            {

                // And getting the id of ego 0
                if (p0.get_id() == p01.get_neighbors()[n0]->get_id())
                    break;            
            }

            // Picking 1's alter
            Agent<TSeq> & p11 = agents->operator[](p1.get_neighbors()[id11]->get_id());
            for (n1 = 0; n1 < p11.get_neighbors().size(); ++n1)
            {

                // And getting the id of ego 1
                if (p1.get_id() == p11.get_neighbors()[n1]->get_id())
                    break;            
            }

            // Swapping alter's endpoints
            std::swap(p01.get_neighbors()[n0], p11.get_neighbors()[n1]);    
            
        }

        // Moving alter first
        std::swap(p0.get_neighbors()[id01], p1.get_neighbors()[id11]);
        

    }

    return;

}

template<typename TSeq>
inline void rewire_degseq(
    AdjList * agents,
    Model<TSeq> * model,
    epiworld_double proportion
    )
{

    // Identifying individuals with degree > 0
    std::vector< int > nties(agents->vcount(), 0); 
    std::vector< unsigned int > non_isolates;
    std::vector< epiworld_double > weights;
    epiworld_double nedges = 0.0;
    // std::vector< Agent<TSeq> > * agents = model->get_agents();
    auto & dat = agents->get_dat();

    for (size_t i = 0u; i < dat.size(); ++i)
        nties[i] += dat[i].size();
    
    bool directed = agents->is_directed();
    for (size_t i = 0u; i < dat.size(); ++i)
    {
        if (nties[i] > 0)
        {
            non_isolates.push_back(i);
            if (directed)
            {
                weights.push_back( 
                    static_cast<epiworld_double>(nties[i])
                );
                nedges += static_cast<epiworld_double>(nties[i]);
            }
            else {
                weights.push_back( 
                    static_cast<epiworld_double>(nties[i])/2.0
                );
                nedges += static_cast<epiworld_double>(nties[i]) / 2.0;
            }
        }
    }

    if (non_isolates.size() == 0u)
        throw std::logic_error("The graph is completely disconnected.");

    // Cumulative probs
    weights[0u] /= nedges;
    for (unsigned int i = 1u; i < non_isolates.size(); ++i)
    {
         weights[i] /= nedges;
         weights[i] += weights[i - 1u];
    }

    // Only swap if needed
    unsigned int N = non_isolates.size();
    epiworld_double prob;
    int nrewires = floor(proportion * nedges / (
        agents->is_directed() ? 1.0 : 2.0
    ));
    while (nrewires-- > 0)
    {

        // Picking egos
        prob = model->runif();
        int id0 = N - 1;
        for (unsigned int i = 0u; i < N; ++i)
            if (prob <= weights[i])
            {
                id0 = i;
                break;
            }

        prob = model->runif();
        int id1 = N - 1;
        for (unsigned int i = 0u; i < N; ++i)
            if (prob <= weights[i])
            {
                id1 = i;
                break;
            }

        // Correcting for under or overflow.
        if (id1 == id0)
            id1++;

        if (id1 >= static_cast<int>(N))
            id1 = 0;

        std::map<unsigned int,unsigned int> & p0 = agents->get_dat()[non_isolates[id0]];
        std::map<unsigned int,unsigned int> & p1 = agents->get_dat()[non_isolates[id1]];

        // Picking alters (relative location in their lists)
        // In this case, these are uniformly distributed within the list
        int id01 = std::floor(p0.size() * model->runif());
        int id11 = std::floor(p1.size() * model->runif());

        // Since it is a map, we need to find the actual ids (positions)
        // are not good enough.
        int count = 0;
        for (auto & n : p0)
            if (count++ == id01)
                id01 = n.first;

        count = 0;
        for (auto & n : p1)
            if (count++ == id11)
                id11 = n.first;

        // When rewiring, we need to flip the individuals from the other
        // end as well, since we are dealing withi an undirected graph
        
        // Finding what neighbour is id0
        if (!agents->is_directed())
        {

            std::map<unsigned int,unsigned int> & p01 = agents->get_dat()[id01];
            std::map<unsigned int,unsigned int> & p11 = agents->get_dat()[id11];

            std::swap(p01[id0], p11[id1]);
            
        }

        // Moving alter first
        std::swap(p0[id01], p1[id11]);

    }

    return;

}

template<typename TSeq>
inline AdjList rgraph_bernoulli(
    unsigned int n,
    epiworld_double p,
    bool directed,
    Model<TSeq> & model
) {

    std::vector< unsigned int > source;
    std::vector< unsigned int > target;

    // Checking the density (how many)
    std::binomial_distribution<> d(
        n * (n - 1.0) / (directed ? 1.0 : 2.0),
        p
    );

    unsigned int m = d(*model.get_rand_endgine());

    source.resize(m);
    target.resize(m);

    int a,b;
    for (unsigned int i = 0u; i < m; ++i)
    {
        a = floor(model.runif() * n);

        if (!directed)
            b = floor(model.runif() * a);
        else
        {
            b = floor(model.runif() * n);
            if (b == a)
                b++;
            
            if (b >= n)
                b = 0u;
        }

        source[i] = a;
        target[i] = b;

    }

    AdjList al(source, target, directed, 0, static_cast<int>(n) - 1);

    return al;
    
}

template<typename TSeq>
inline AdjList rgraph_bernoulli2(
    unsigned int n,
    epiworld_double p,
    bool directed,
    Model<TSeq> & model
) {

    std::vector< unsigned int > source;
    std::vector< unsigned int > target;

    // Checking the density (how many)
    std::binomial_distribution<> d(
        n * (n - 1.0) / (directed ? 1.0 : 2.0),
        p
    );

    // Need to compensate for the possible number of diagonal
    // elements sampled. If n * n, then each diag element has
    // 1/(n^2) chance of sampling

    unsigned int m = d(*model.get_rand_endgine());

    source.resize(m);
    target.resize(m);

    double n2 = static_cast<double>(n * n);

    int loc,row,col;
    for (unsigned int i = 0u; i < m; ++i)
    {
        loc = floor(model.runif() * n2);
        col = floor(static_cast<double>(loc)/static_cast<double>(n));
        row = loc - row * n;

        // Undirected needs to swap
        if (!directed && (col > row))
            std::swap(col, row);

        source[i] = row;
        target[i] = col;

    }

    AdjList al(source, target, directed, 0, static_cast<int>(n) - 1);

    return al;
    
}

inline AdjList rgraph_ring_lattice(
    unsigned int n,
    unsigned int k,
    bool directed = false
) {

    if ((n - 1u) < k)
        throw std::logic_error("k can be at most n - 1.");

    std::vector< unsigned int > source;
    std::vector< unsigned int > target;

    // if (!directed)
    //     if (k > 1u) k = static_cast< unsigned int >(floor(k / 2.0));

    for (unsigned int i = 0; i < n; ++i)
    {

        for (unsigned int j = 1u; j <= k; ++j)
        {

            // Next neighbor
            unsigned int l = i + j;
            if (l >= n) l = l - n;

            source.push_back(i);
            target.push_back(l);

        }

    }

    return AdjList(source, target, n, directed);

}

template<typename TSeq>
inline AdjList rgraph_smallworld(
    unsigned int n,
    unsigned int k,
    epiworld_double p,
    bool directed,
    Model<TSeq> & model
) {

    // Creating the ring lattice
    AdjList ring = rgraph_ring_lattice(n,k,directed);
    
    // Rewiring and returning
    if (k > 0u)
        rewire_degseq(&ring, &model, p);
        
    return ring;

}

#endif
