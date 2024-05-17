// Copyright (C) 2024 Moritz Scheer

TEST(ServerLoopTest, QueueInitializationFailure) {
	server server;
	uring uring;
	struct io_uring ring;

	// Mock the io_uring_queue_init function to return an error
	io_uring_queue_init_mock.expect_any_call().and_return(-1);

	// Call the server_loop function
	int result = server_loop(&server, &uring);

	// Verify that the function returns the error code
	ASSERT_EQ(result, -1);

	// Verify that the perror function is called with the correct error message
	perror_mock.expect_any_call("Could not initialize io_uring queue.");
}

TEST(ServerLoopTest, WaitForCompletionFailure) {
	server server;
	uring uring;
	struct io_uring ring;
	struct io_uring_cqe cqe;

	// Mock the io_uring_queue_init function to return success
	io_uring_queue_init_mock.expect_any_call().and_return(0);

	// Mock the io_uring_wait_cqe function to return an error
	io_uring_wait_cqe_mock.expect_any_call(&ring, &cqe).and_return(-1);

	// Call the server_loop function
	int result = server_loop(&server, &uring);

	// Verify that the function returns an error code
	ASSERT_NE(result, 0);

	// Verify that the perror function is called with the correct error message
	perror_mock.expect_any_call("waiting for completion");
}

TEST(ServerLoopTest, SubmissionQueueFull) {
	server server;
	uring uring;
	struct io_uring ring;
	struct io_uring_cqe cqe;

	// Mock the io_uring_queue_init function to return success
	io_uring_queue_init_mock.expect_any_call().and_return(0);

	// Mock the io_uring_wait_cqe function to return success
	io_uring_wait_cqe_mock.expect_any_call(&ring, &cqe).and_return(0);

	// Mock the io_uring_sq_space_left function to return 0 (queue full)
	io_uring_sq_space_left_mock.expect_any_call(&ring).and_return(0);

	// Call the server_loop function
	int result = server_loop(&server, &uring);

	// Verify that the function returns success
	ASSERT_EQ(result, 0);

	// Verify that the io_uring_submit function is not called
	io_uring_submit_mock.expect_any_call(&ring).never();
}

TEST(ServerLoopTest, NoRemainingWork) {
	server server;
	uring uring;
	struct io_uring ring;
	struct io_uring_cqe cqe;

	// Mock the io_uring_queue_init function to return success
	io_uring_queue_init_mock.expect_any_call().and_return(0);

	// Mock the io_uring_wait_cqe function to return success
	io_uring_wait_cqe_mock.expect_any_call(&ring, &cqe).and_return(0);

	// Mock the io_uring_peek_cqe function to return -EAGAIN (no remaining work)
	io_uring_peek_cqe_mock.expect_any_call(&ring, &cqe).and_return(-EAGAIN);

	// Call the server_loop function
	int result = server_loop(&server, &uring);

	// Verify that the function returns success
	ASSERT_EQ(result, 0);

	// Verify that the io_uring_submit function is not called
	io_uring_submit_mock.expect_any_call(&ring).never();
}

TEST(ServerLoopTest, QueueExitSuccess) {
    server server;
    uring uring;
    struct io_uring ring;
    struct io_uring_cqe cqe;

    // Mock the io_uring_queue_init function to return success
    io_uring_queue_init_mock.expect_any_call().and_return(0);

    // Mock the io_uring_wait_cqe function to return success
    io_uring_wait_cqe_mock.expect_any_call(&ring, &cqe).and_return(0);

    // Mock the io_uring_peek_cqe function to return -EAGAIN (no remaining work)
    io_uring_peek_cqe_mock.expect_any_call(&ring, &cqe).and_return(-EAGAIN);

    // Call the server_loop function
    int result = server_loop(&server, &uring);

    // Verify that the function returns success
    ASSERT_EQ(result, 0);

    // Verify that the io_uring_queue_exit function is called
    io_uring_queue_exit_mock.expect_any_call(&ring);
}

