/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_CORE_REFERENCE_MANAGER_H_
#define SRC_GENIE_CORE_REFERENCE_MANAGER_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <algorithm>
#include <deque>
#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <utility>
#include <vector>
#include "genie/core/meta/reference.h"
#include "genie/core/reference_collection.h"
#include "genie/util/make_unique.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {

/**
 * @brief
 */
class ReferenceManager {
 private:
    ReferenceCollection mgr_;  //!< @brief

    struct CacheLine {
        std::shared_ptr<const std::string> chunk;  //!< @brief
        std::weak_ptr<const std::string> memory;   //!< @brief
        std::mutex load_mutex;                      //!< @brief
    };

    std::map<std::string, std::vector<std::unique_ptr<CacheLine>>> data_;  //!< @brief
    std::map<size_t, std::string> indices_;
    std::deque<std::pair<std::string, size_t>> cache_info_;  //!< @brief
    std::mutex cache_info_lock_;                              //!< @brief
    uint64_t cache_size_;                                    //!< @brief
    static const uint64_t chunk_size_;                      //!< @brief

    /**
     * @brief
     * @param name
     * @param num
     */
    void touch(const std::string& name, size_t num);

 public:
    /**
     * @brief
     * @param id
     */
    void validateRefID(size_t id);

    /**
     * @brief
     * @param ref
     * @return
     */
    size_t Ref2Id(const std::string& ref);

    /**
     * @brief
     * @param id
     * @return
     */
    std::string Id2Ref(size_t id);

    /**
     * @brief
     * @param id
     * @return
     */
    bool RefKnown(size_t id);

    /**
     * @brief
     * @return
     */
    static size_t GetChunkSize();

    /**
     * @brief
     */
    struct ReferenceExcerpt {
     private:
        std::string ref_name;                                  //!< @brief
        size_t global_start;                                   //!< @brief
        size_t global_end;                                     //!< @brief
        std::vector<std::shared_ptr<const std::string>> data;  //!< @brief

     public:
        /**
         * @brief
         * @return
         */
        bool IsEmpty() const;

        /**
         * @brief
         * @param e
         */
        void Merge(ReferenceExcerpt& e);

        /**
         * @brief
         * @return
         */
        size_t GetGlobalStart() const;

        /**
         * @brief
         * @return
         */
        size_t GetGlobalEnd() const;

        /**
         * @brief
         * @return
         */
        const std::string& GetRefName() const;

        /**
         * @brief
         * @param pos
         * @return
         */
        std::shared_ptr<const std::string> GetChunkAt(size_t pos) const;

        /**
         * @brief
         * @param pos
         * @param dat
         */
        void mapChunkAt(size_t pos, std::shared_ptr<const std::string> dat);

        /**
         * @brief
         */
        ReferenceExcerpt();

        /**
         * @brief
         * @param name
         * @param start
         * @param end
         */
        ReferenceExcerpt(std::string name, size_t start, size_t end);

        /**
         * @brief
         * @return
         */
        static const std::shared_ptr<const std::string>& undef_page();

        /**
         * @brief
         * @param newEnd
         */
        void extend(size_t newEnd);

        /**
         * @brief
         * @param page
         * @return
         */
        static bool isMapped(const std::shared_ptr<const std::string>& page);

        /**
         * @brief
         * @param pos
         * @return
         */
        bool isMapped(size_t pos) const;

        /**
         * @brief
         */
        struct Stepper {
         private:
            std::vector<std::shared_ptr<const std::string>>::const_iterator startVecIt;  //!< @brief
            std::vector<std::shared_ptr<const std::string>>::const_iterator vecIt;       //!< @brief
            std::vector<std::shared_ptr<const std::string>>::const_iterator endVecIt;    //!< @brief
            size_t stringPos;                                                            //!< @brief
            const char* curString;                                                       //!< @brief

         public:
            /**
             * @brief
             * @param e
             */
            explicit Stepper(const ReferenceExcerpt& e);

            /**
             * @brief
             * @param off
             */
            void inc(size_t off = 1);

            /**
             * @brief
             * @return
             */
            bool end() const;

            /**
             * @brief
             * @return
             */
            char get() const;
        };

        /**
         * @brief
         * @return
         */
        Stepper getStepper() const;

        /**
         * @brief
         * @param start
         * @param end
         * @return
         */
        std::string getString(size_t start, size_t end) const;
    };

    /**
     * @brief
     * @param csize
     */
    explicit ReferenceManager(size_t csize);

    /**
     * @brief
     * @param index
     * @param ref
     */
    void addRef(size_t index, std::unique_ptr<Reference> ref);

    /**
     * @brief
     * @param name
     * @param pos
     * @return
     */
    std::shared_ptr<const std::string> loadAt(const std::string& name, size_t pos);

    /**
     * @brief
     * @param name
     * @param start
     * @param end
     * @return
     */
    ReferenceExcerpt Load(const std::string& name, size_t start, size_t end);

    /**
     * @brief
     * @param name
     * @return
     */
    std::vector<std::pair<size_t, size_t>> GetCoverage(const std::string& name) const;

    /**
     * @brief
     * @return
     */
    std::vector<std::string> GetSequences() const;

    /**
     * @brief
     * @param name
     * @return
     */
    size_t GetLength(const std::string& name);
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_REFERENCE_MANAGER_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
