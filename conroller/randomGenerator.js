export const generateRandomValue = () => {
  // Generate a random value between 0 and 10
  const randomValue = Math.random() * 10;

  // Round the random value to 2 decimal places
  const roundedValue = Number(randomValue.toFixed(2));

  return roundedValue;
};
