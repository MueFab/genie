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
#include "genie/core/reference-collection.h"
#include "genie/util/make-unique.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {

/**
 *
 */
class ReferenceManager {
 private:
    ReferenceCollection mgr;  //!<

    struct CacheLine {
        std::shared_ptr<const std::string> chunk;  //!<
        std::weak_ptr<const std::string> memory;   //!<
        std::mutex loadMutex;                      //!<
    };

    std::map<std::string, std::vector<std::unique_ptr<CacheLine>>> data;  //!<
    std::map<size_t, std::string> indices;
    std::deque<std::pair<std::string, size_t>> cacheInfo;  //!<
    std::mutex cacheInfoLock;                              //!<
    uint64_t cacheSize;                                    //!<
    static const uint64_t CHUNK_SIZE;                      //!<

    /**
     *
     * @param name
     * @param num
     */
    void touch(const std::string& name, size_t num);

 public:
    /**
     *
     * @param id
     */
    void validateRefID(size_t id);

    /**
     *
     * @param ref
     * @return
     */
    size_t ref2ID(const std::string& ref);

    /**
     *
     * @param id
     * @return
     */
    std::string ID2Ref(size_t id);

    bool refKnown(size_t id);

    /**
     *
     * @return
     */
    static size_t getChunkSize();

    /**
     *
     */
    struct ReferenceExcerpt {
     private:
        std::string ref_name;                                  //!<
        size_t global_start;                                   //!<
        size_t global_end;                                     //!<
        std::vector<std::shared_ptr<const std::string>> data;  //!<

     public:
        /**
         *
         * @return
         */
        bool isEmpty() const;

        /**
         *
         * @param e
         */
        void merge(ReferenceExcerpt& e);

        /**
         *
         * @return
         */
        size_t getGlobalStart() const;

        /**
         *
         * @return
         */
        size_t getGlobalEnd() const;

        /**
         *
         * @return
         */
        const std::string& getRefName() const;

        /**
         *
         * @param pos
         * @return
         */
        std::shared_ptr<const std::string> getChunkAt(size_t pos) const;

        /**
         *
         * @param pos
         * @param dat
         */
        void mapChunkAt(size_t pos, std::shared_ptr<const std::string> dat);

        /**
         *
         */
        ReferenceExcerpt();

        /**
         *
         * @param name
         * @param start
         * @param end
         */
        ReferenceExcerpt(std::string name, size_t start, size_t end);

        /**
         *
         * @return
         */
        static const std::shared_ptr<const std::string>& undef_page();

        /**
         *
         * @param newEnd
         */
        void extend(size_t newEnd);

        /**
         *
         * @param page
         * @return
         */
        static bool isMapped(const std::shared_ptr<const std::string>& page);

        /**
         *
         * @param pos
         * @return
         */
        bool isMapped(size_t pos) const;

        /**
         *
         */
        struct Stepper {
         private:
            std::vector<std::shared_ptr<const std::string>>::const_iterator startVecIt;  //!<
            std::vector<std::shared_ptr<const std::string>>::const_iterator vecIt;       //!<
            std::vector<std::shared_ptr<const std::string>>::const_iterator endVecIt;    //!<
            size_t stringPos;                                                            //!<
            const char* curString;                                                       //!<

         public:
            /**
             *
             * @param e
             */
            explicit Stepper(const ReferenceExcerpt& e);

            /**
             *
             * @param off
             */
            void inc(size_t off = 1);

            /**
             *
             * @return
             */
            bool end() const;

            /**
             *
             * @return
             */
            char get() const;
        };

        /**
         *
         * @return
         */
        Stepper getStepper() const;

        /**
         *
         * @param start
         * @param end
         * @return
         */
        std::string getString(size_t start, size_t end) const;
    };

    /**
     *
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
     *
     * @param name
     * @param pos
     * @return
     */
    std::shared_ptr<const std::string> loadAt(const std::string& name, size_t pos);

    /**
     *
     * @param name
     * @param start
     * @param end
     * @return
     */
    ReferenceExcerpt load(const std::string& name, size_t start, size_t end);

    /**
     *
     * @param name
     * @return
     */
    std::vector<std::pair<size_t, size_t>> getCoverage(const std::string& name) const;

    /**
     *
     * @return
     */
    std::vector<std::string> getSequences() const;

    /**
     *
     * @param name
     * @return
     */
    size_t getLength(const std::string& name);
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_REFERENCE_MANAGER_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
