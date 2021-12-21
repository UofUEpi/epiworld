#ifndef EPIWORLD_DATABASE_BONES_HPP
#define EPIWORLD_DATABASE_BONES_HPP

template<typename TSeq>
class Model;

template<typename TSeq>
class Virus;

/**
 * @brief Hasher function to turn the sequence into an integer vector
 * 
 * @tparam TSeq 
 * @param x 
 * @return std::vector<int> 
 */
template<typename TSeq>
inline std::vector<int> default_seq_hasher(const TSeq & x);

template<>
inline std::vector<int> default_seq_hasher<std::vector<int>>(const std::vector<int> & x) {
    return x;
}

template<>
inline std::vector<int> default_seq_hasher<std::vector<bool>>(const std::vector<bool> & x) {
    std::vector<int> ans;
    for (const auto & i : x)
        ans.push_back(i? 1 : 0);
    return ans;
}

template<>
inline std::vector<int> default_seq_hasher<int>(const int & x) {
    return {x};
}

template<>
inline std::vector<int> default_seq_hasher<bool>(const bool & x) {
    return {x ? 1 : 0};
}

/**
 * @brief Default way to write sequences
 * 
 * @tparam TSeq 
 * @param seq 
 * @return std::string 
 */
template<typename TSeq>
inline std::string default_seq_writer(const TSeq & seq);

template<>
inline std::string default_seq_writer<std::vector<int>>(
    const std::vector<int> & seq
) {

    std::string out = "";
    for (const auto & s : seq)
        out = out + std::to_string(s);

    return out;

}

template<>
inline std::string default_seq_writer<std::vector<bool>>(
    const std::vector<bool> & seq
) {

    std::string out = "";
    for (const auto & s : seq)
        out = out + (s ? "1" : "0");

    return out;

}

template<>
inline std::string default_seq_writer<bool>(
    const bool & seq
) {

    return seq ? "1" : "0";

}

template<>
inline std::string default_seq_writer<int>(
    const int & seq
) {

    return std::to_string(seq);

}

/**
 * @brief Statistical data about the process
 * 
 * @tparam TSeq 
 */
template<typename TSeq>
class DataBase {
private:
    Model<TSeq> * model;

    // Variants information 
    MapVec_type<int,int> variant_id; ///< The squence is the key
    std::vector< TSeq> sequence;
    std::vector< int > origin_date;
    std::vector< int > parent_id;

    std::function<std::vector<int>(const TSeq&)> seq_hasher = default_seq_hasher<TSeq>;
    std::function<std::string(const TSeq &)> seq_writer = default_seq_writer<TSeq>;

    // Running sum of the variant's information
    std::vector< int > today_variant_ninfected;  ///< Running sum
    std::vector< int > today_variant_nrecovered; ///< Running sum
    std::vector< int > today_variant_ndeceased;  ///< Running sum

    // Totals
    int today_total_nvariants_active = 0;
    int today_total_nhealthy   = 0;
    int today_total_nrecovered = 0;
    int today_total_ninfected  = 0;
    int today_total_ndeceased  = 0;
    
    int sampling_freq;

    // Variants history
    std::vector< int > hist_variant_date;
    std::vector< int > hist_variant_id;
    std::vector< int > hist_variant_ninfected;
    std::vector< int > hist_variant_nrecovered;
    std::vector< int > hist_variant_ndeceased;

    // Overall hist
    std::vector< int > hist_total_date;
    std::vector< int > hist_total_nvariants_active;
    std::vector< int > hist_total_nhealthy;
    std::vector< int > hist_total_nrecovered;
    std::vector< int > hist_total_ninfected;
    std::vector< int > hist_total_ndeceased;  

    // Transmission network
    std::vector< int > transmision_date;
    std::vector< int > transmision_source;
    std::vector< int > transmision_target;
    std::vector< int > transmision_variant;

public:

    DataBase(int freq = 1) : sampling_freq(freq) {};

    /**
     * @brief Registering a new variant
     * 
     * @param v Pointer to the new variant.
     * Since variants are originated in the host, the numbers simply move around.
     * From the parent variant to the new variant. And the total number of infected
     * does not change.
     */
    void record_variant(Virus<TSeq> * v); 
    void set_seq_hasher(std::function<std::vector<int>(TSeq)> fun);
    void set_model(Model<TSeq> & m);
    void record();

    const std::vector< TSeq > & get_sequence() const;
    const std::vector< int > & get_ninfected() const;
    size_t size() const;

    void up_infected(Virus<TSeq> * v);
    void up_recovered(Virus<TSeq> * v);
    void up_deceased(Virus<TSeq> * v);

    int get_today_total(std::string what) const;
    const std::vector< int > & get_today_variant(std::string what) const;
    const std::vector< int > & get_hist_total(std::string what) const;
    const std::vector< int > & get_hist_variant(std::string what) const;

    void write_data(
        std::string fn_variant_info,
        std::string fn_variant_hist,
        std::string fn_total_hist,
        std::string fn_transmision
        ) const;
    
    void record_transmision(int i, int j, int variant);
};


#endif