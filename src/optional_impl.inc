#ifdef SUPPORT_OPTIONAL
		template<typename T, std::enable_if_t<(hdy::type::traits::is_string_v<T> || std::is_arithmetic_v<T>)
			&& !hdy::type::traits::is_container_v<T>, int> = 0>
		Assign operator=(const std::optional<T>& value) const {
			if (!value.has_value()) {
				return Assign();
			}
			return Assign(_name, format_value(value));
		}

		template<typename T, std::enable_if_t<(hdy::type::traits::is_string_v<T> || std::is_arithmetic_v<T>) && !hdy::type::traits::is_container_v<T>, int> = 0>
		Condition operator==(const std::optional<T> &value) const {
			if (!value.has_value()) {
				return Condition();
			}
			return *this == *value;
		}

		template<typename T, std::enable_if_t<(hdy::type::traits::is_string_v<T> || std::is_arithmetic_v<T>) && !hdy::type::traits::is_container_v<T>, int> = 0>
		Condition operator>(const std::optional<T> &value) const {
			if (!value.has_value()) {
				return Condition();
			}
			return *this > *value;
		}

		template<typename T, std::enable_if_t<(hdy::type::traits::is_string_v<T> || std::is_arithmetic_v<T>) && !hdy::type::traits::is_container_v<T>, int> = 0>
		Condition operator>=(const std::optional<T> &value) const {
			if (!value.has_value()) {
				return Condition();
			}
			return *this >= *value;
		}

		template<typename T, std::enable_if_t<(hdy::type::traits::is_string_v<T> || std::is_arithmetic_v<T>) && !hdy::type::traits::is_container_v<T>, int> = 0>
		Condition operator<(const std::optional<T> &value) const {
			if (!value.has_value()) {
				return Condition();
			}
			return *this < *value;
		}

		template<typename T, std::enable_if_t<(hdy::type::traits::is_string_v<T> || std::is_arithmetic_v<T>) && !hdy::type::traits::is_container_v<T>, int> = 0>
		Condition operator<=(const std::optional<T> &value) const {
			if (!value.has_value()) {
				return Condition();
			}
			return *this <= *value;
		}

		template<typename T, std::enable_if_t<(hdy::type::traits::is_string_v<T> || std::is_arithmetic_v<T>) && !hdy::type::traits::is_container_v<T>, int> = 0>
		Condition operator!=(const std::optional<T>& value) const {
			if (!value.has_value()) {
				return Condition();
			}
			return *this != *value;
		}
		/*
		Condition operator==(std::nullopt_t) const {
			return Condition();
		}
		Condition operator!=(std::nullopt_t) const {
			return Condition();
		}
		Condition operator>(std::nullopt_t) const {
			return Condition();
		}
		Condition operator>=(std::nullopt_t) const {
			return Condition();
		}
		Condition operator<(std::nullopt_t) const {
			return Condition();
		}
		Condition operator<=(std::nullopt_t) const {
			return Condition();
		}*/
		//ename LIKE '张%'
		Condition like(const std::optional<std::string>& value, std::string_view fmt = "%{}%") const 
		{
			if (value.has_value()) {
				return like(*value, fmt);
			}
			return Condition();
		}

		template<typename T, std::enable_if_t<(std::is_arithmetic_v<T> || hdy::type::traits::is_string_v<T>) && !hdy::type::traits::is_container_v<T>, int> = 0>
		Condition between_and(const std::optional<T>& lower, const std::optional<T>& upper) const {
		    if (!lower.has_value() || !upper.has_value()) {
				return Condition();  // 与其他 optional 操作保持一致
			}
			return Condition(_name, "BETWEEN", std::format("{} AND {}", format_value(lower), format_value(upper)));
		}
#endif
