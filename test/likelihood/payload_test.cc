
TEST(Likelihood, PayloadRoundTrip) {
    uint32_t nrows = 10;
    uint32_t ncols = 5;
    std::vector<uint8_t> payload = {1, 2, 3, 4, 5};
    std::vector<uint8_t> additionalPayload = {6, 7};
    bool transform_flag = true;

    genie::likelihood::LikelihoodParameters params; 
    // params doesn't store the payloads, just metadata. 
    // LikelihoodPayload constructor takes params just to check transform_flag usually?
    // Actually constructor: 
    // LikelihoodPayload(LikelihoodParameters _parameters, uint32_t _nrows, uint32_t _ncols, vector _payload, vector _add);
    // But it ignores _parameters in the body! (void)_parameters;
    // Except it sets transform_flag = false in initializer list!
    // And setTransformFlag exists.

    // Let's use the setter approach or constructor.
    // The constructor sets transform_flag = false.
    genie::likelihood::LikelihoodPayload inputPayload(params, nrows, ncols, payload, additionalPayload);
    inputPayload.setTransformFlag(transform_flag); 

    std::stringstream ss;
    genie::util::BitWriter writer(ss);
    inputPayload.write(writer);
    writer.FlushBits();

    genie::util::BitReader reader(ss);
    genie::likelihood::LikelihoodPayload outputPayload(params, 0, 0, {}, {}); // Dummy init
    outputPayload.setTransformFlag(transform_flag); // Must set this before read!
    outputPayload.read(reader);

    ASSERT_EQ(outputPayload.getNRows(), nrows);
    ASSERT_EQ(outputPayload.getNCols(), ncols);
    ASSERT_EQ(outputPayload.getPayload(), payload);
    ASSERT_EQ(outputPayload.getAdditionalPayload(), additionalPayload);
}
