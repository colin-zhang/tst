#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <pthread.h>

#include <string>
#include <vector>
#include <map>

#include "filter_policy.h"

class CustomFilterPolicy : public FilterPolicy {
   private:
    const FilterPolicy* builtin_policy_;
   public:
    CustomFilterPolicy() : builtin_policy_(NewBloomFilterPolicy(12)) { }
    ~CustomFilterPolicy() { delete builtin_policy_; }

    const char* Name() const { return "IgnoreTrailingSpacesFilter"; }

    void CreateFilter(const std::string* keys, int n, std::string* dst) const {
/*      
      // Use builtin bloom filter code after removing trailing spaces
      std::vector<Slice> trimmed(n);
      for (int i = 0; i < n; i++) {
        trimmed[i] = RemoveTrailingSpaces(keys[i]);
      }
*/
      return builtin_policy_->CreateFilter(keys, n, dst);
    }

    bool KeyMayMatch(const std::string& key, const std::string& filter) const {
      // Use builtin bloom filter code after removing trailing spaces
      return builtin_policy_->KeyMayMatch(key, filter);
    }
};

static pthread_mutex_t m_init_lock;


static uint64_t getCurrVs()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec * 1000 + tv.tv_usec / 1000;
}


int main(int argc, char const *argv[])
{
    std::string filter;
    std::vector<std::string> keys;

    //std::map<char*, uint64_t> key_values;
    //std::map<uint64_t, uint64_t> key_values;
    std::map<std::string, uint64_t> key_values;

    size_t n = 0;
    size_t N = 10000;
    size_t NN = N*20000;

    size_t match = 0;
    size_t unmatch = 0;
    char buf[128];

    CustomFilterPolicy* bloom = new CustomFilterPolicy;
    pthread_mutex_init(&m_init_lock, NULL);

    for (n = 0; n < N; n++) {
        snprintf(buf, sizeof(buf), "KeyMayMatch %lu", n);
        keys.push_back(buf);
    }

    //keys_.push()
    uint64_t a, b;
    a = getCurrVs();
    match = 0;
    unmatch = 0;
    bloom->CreateFilter(&keys[0], keys.size(), &filter);

//    std::vector<std::string>::iterator it;
/*
    for (it = keys.begin(); it != keys.end(); it++ ) {
        if (bloom->KeyMayMatch(*it, filter)) {
            match++;
        } else {
            unmatch++;
        }
    }
*/

    for (n = N; n < NN; n++) {
        //pthread_mutex_lock(&m_init_lock);
        snprintf(buf, sizeof(buf), "KeyMayMatch %lu", n);
        std::string xxx(buf);
        if (bloom->KeyMayMatch(xxx, filter)) {
            match++;
        } else {
            unmatch++;
        }
        //pthread_mutex_unlock(&m_init_lock);
    }
    b = getCurrVs();

    printf("bloom:\n");
    printf("unmatch(%lu), match(%lu), NN=%lu, filter size=%lu \n", unmatch, match, NN, filter.size());
    printf("unmatch/NN=%lf, match/NN=%lf\n", (double)unmatch/NN, (double)match/NN);
    printf("time = %lu \n\n", b - a);


    a = getCurrVs();
    match = 0;
    unmatch = 0;
    for (n = 0; n < N; n++) {
        snprintf(buf, sizeof(buf), "KeyMayMatch %lu", n);
        std::string xxx1(buf);
        //printf("buf:: %s\n", buf);
        //key_values[buf] = n;
        key_values[xxx1] = n;
        //key_values[n] = n;
    }

    for (n = N; n < NN; n++) {
        //snprintf(buf, sizeof(buf), "%lu", n);
        snprintf(buf, sizeof(buf), "KeyMayMatch %lu", n);
        std::string xxx2(buf);
        if (key_values.find(xxx2) == key_values.end()) {
            match++;
        } else {
            unmatch++;
        }
    }
    b = getCurrVs();
    printf("map:\n");
    printf("unmatch(%lu), match(%lu), NN=%lu, map size=%lu \n", unmatch, match, NN, key_values.size());
    printf("time = %lu \n\n", b - a);

    delete bloom;
    pthread_mutex_destroy(&m_init_lock);
    return 0;
}
