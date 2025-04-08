package Server.models;

/**
 * Rating class represents the rating system for a facility.
 * It tracks the total rating sum, number of ratings, and calculates the average
 * rating.
 */
public class Rating {
    private double totalRatingSum; // Sum of all ratings
    private int numberOfRatings; // Total number of ratings
    private double averageRating; // Average rating

    /**
     * Constructor to initialize the Rating object.
     */
    public Rating() {
        this.totalRatingSum = 0.0;
        this.numberOfRatings = 0;
        this.averageRating = 0.0;
    }

    /**
     * Add a new rating and update the average rating.
     * 
     * @param rating The new rating to add.
     */
    public void addRating(double rating) {
        this.totalRatingSum += rating;
        this.numberOfRatings++;
        if (numberOfRatings == 0) {
            this.averageRating = 0.0;
            return;
        }

        this.averageRating = this.totalRatingSum / this.numberOfRatings;
        
    }

    /**
     * Calculate the average rating based on the total rating sum and number of
     * ratings.
     */
    private void calculateAverageRating() {
        if (numberOfRatings == 0) {
            this.averageRating = 0.0;
            return;
        }

        this.averageRating = this.totalRatingSum / this.numberOfRatings;

    }

    /**
     * Get the average rating.
     * 
     * @return The average rating.
     */
    public double getAverageRating() {
        return this.averageRating;
    }

    /**
     * Get the total number of ratings.
     * 
     * @return The number of ratings.
     */
    public int getNumberOfRatings() {
        return this.numberOfRatings;
    }

    /**
     * Get the total sum of all ratings.
     * 
     * @return The total rating sum.
     */
    public double getTotalRatingSum() {
        return this.totalRatingSum;
    }

    /**
     * Convert the Rating object to a string representation.
     * 
     * @return A string in the format "Rating: AVG (NUM ratings)".
     */
    @Override
    public String toString() {
        return "Rating: " + this.averageRating + " (" + this.numberOfRatings + " ratings)";
    }
}