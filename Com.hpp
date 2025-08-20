#pragma once

class COM
{
public:
	COM();

	COM(COM&&) = delete;
	COM& operator=(COM&&) = delete;

	~COM();
};
